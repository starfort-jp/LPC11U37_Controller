/***********************************************************************
* $Id:: main.c 221 2012-01-13 02:54:53Z usb06052                              $
*
* Project: USB device ROM Stack test module
*
* Description:
*     USB Communication Device Class User module.
*
***********************************************************************
*   Copyright(C) 2011, NXP Semiconductor
*   All rights reserved.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#include <string.h>
#include "LPC11Uxx.h"            
#include "power_api.h"
#include "mw_usbd_rom_api.h"
#include "usb_rom_func.h"
#include "cappuccino.h"
#include "i2c.h"
#include "i2clcd.h"
#include "type.h"

#define UART_BRIDGE 1

extern uint8_t VCOM_DeviceDescriptor[];
extern uint8_t VCOM_StringDescriptor[];
extern uint8_t VCOM_ConfigDescriptor[];

USBD_API_T* pUsbApi;

/* UART defines */
#define TX_FIFO_SIZE 16

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

/* VCOM defines */
#define VCOM_BUFFERS    4
#define VCOM_BUF_EMPTY_INDEX  (0xFF)
#define VCOM_BUF_FREE   0
#define VCOM_BUF_ALLOC  1
#define VCOM_BUF_USBTXQ  2
#define VCOM_BUF_UARTTXQ  3
#define VCOM_BUF_ALLOCU  4

char xData = '>';

VCOM_DATA_T g_vCOM;
ErrorCode_t VCOM_bulk_out_hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event);
/**********************************************************************
 ** Routines
**********************************************************************/
// static---
static void uart_write(VCOM_DATA_T* pVcom)
{
  uint8_t *pbuf = pVcom->rxBuf;
  uint32_t tx_cnt =  TX_FIFO_SIZE;  // find space in TX FIFO
  tx_cnt = 0xF - (tx_cnt & 0xF);
  if (tx_cnt > (pVcom->rxlen - pVcom->ser_pos))
  {
    tx_cnt = (pVcom->rxlen - pVcom->ser_pos);
  }
  while(tx_cnt)
  {
      if (LPC_USART->LSR & LSR_THRE)
      {
        LPC_USART->THR = pbuf[pVcom->ser_pos++];
        tx_cnt--;
      }
  }
  if (pVcom->ser_pos == pVcom->rxlen)	// if done check anything pending
  {
    pVcom->ser_pos = 0;		// TX complete free the buffer
    pVcom->rxlen = 0;
    if(pVcom->usbrx_pend)
    {
      pVcom->usbrx_pend = 0;
      VCOM_Bulk_Out_Hdlr(pVcom->hUsb, (void*)pVcom, USB_EVT_OUT);
    }
  }
  return ;
}

static void uart_read(VCOM_DATA_T* pVcom)
{
  uint8_t *pbuf;

  pbuf = pVcom->txBuf;

  if( (LPC_USART->LSR & LSR_RDR) && (pVcom->txlen < USB_HS_MAX_BULK_PACKET) ) {
    pbuf[pVcom->txlen++] = LPC_USART->RBR;
  }

  if (pVcom->txlen == USB_HS_MAX_BULK_PACKET) {
	  VCOM_USB_Send(pVcom);
  }
  pVcom->last_ser_rx = pVcom->sof_counter;
}

// Initialization---
void UART_Bridge_Init(VCOM_DATA_T* pVcom, CDC_LINE_CODING* line_coding)
{
  uint32_t Fdiv, baud = 9600;
  uint8_t  lcr = 0x3;    	// 8 bits, no Parity, 1 Stop bit
  if (line_coding)
  {
    if ( line_coding->bCharFormat)
    {
        lcr |= (1 << 2);             // Number of stop bits
    }
    if ( line_coding->bParityType)	 // Parity bit type
    {
      lcr |= (1 << 3);
      lcr |=  (((line_coding->bParityType - 1) & 0x3) << 4);
    }
    if ( line_coding->bDataBits)	 // Number of data bits
    {
      lcr |= ((line_coding->bDataBits - 5) & 0x3);
    }
    else
    {
      lcr |= 0x3;
    }
    baud = line_coding->dwDTERate;
    // enable SOF after we are connected
    pUsbApi->hw->EnableEvent(pVcom->hUsb, 0, USB_EVT_SOF, 1);
  }
  else   // Enable UART clock, UART I/O configuration
  {
	  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
	  LPC_SYSCON->UARTCLKDIV = 0x1;		// divided by 1
	  LPC_IOCON->PIO1_26 &= ~0x07;
	  LPC_IOCON->PIO1_26 |= 0x02;     	// UART RXD
	  LPC_IOCON->PIO1_27 &= ~0x07;
	  LPC_IOCON->PIO1_27 |= 0x02;     	// UART TXD
  } 
  Fdiv = ( (SystemCoreClock/LPC_SYSCON->UARTCLKDIV) / 16 ) / baud ;	// baud rate
  LPC_USART->IER = 0;
  LPC_USART->LCR = lcr | 0x80;	// DLAB = 1
  LPC_USART->DLM = Fdiv / 256;							
  LPC_USART->DLL = Fdiv % 256;
  LPC_USART->LCR = lcr;			// DLAB = 0
  LPC_USART->FCR = 0x07;		// Enable and reset TX and RX FIFO Rx trigger level 4 chars
  LPC_USART->IER = IER_RBR | IER_THRE | IER_RLS;	// Enable UART1 interrupt
}

void USB_Pin_Clk_Init(void)
{
  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
  /* Enable AHB clock to the USB block and USB RAM. */
  LPC_SYSCON->SYSAHBCLKCTRL |= ((0x1<<14)|(0x1<<27));
  /* Pull-down is needed, or internally, VBUS will be floating. This is to
  address the wrong status in VBUSDebouncing bit in CmdStatus register. It
  happens on the NXP Validation Board only that a wrong ESD protection chip is used. */
//  LPC_IOCON->PIO0_3   &= ~0x1F;
//  LPC_IOCON->PIO0_3   |= ((0x1<<3)|(0x01<<0));	/* Secondary function VBUS */
//  LPC_IOCON->PIO0_3   |= (0x01<<0);			/* Secondary function VBUS */
  LPC_IOCON->PIO0_6   &= ~0x07;
  LPC_IOCON->PIO0_6   |= (0x01<<0);			/* Secondary function SoftConn */
  return;
}

void USB_CDC_Init (void)
{
  USBD_API_INIT_PARAM_T usb_param;
  USBD_CDC_INIT_PARAM_T cdc_param;
  USB_CORE_DESCS_T desc;
  USBD_HANDLE_T hUsb, hCdc;
  ErrorCode_t ret = LPC_OK;
  uint32_t ep_indx;
  SystemCoreClockUpdate ();
// get USB API table pointer
  pUsbApi = (USBD_API_T*)((*(ROM **)(0x1FFF1FF8))->pUSBD);
// enable clocks and pin multiplexer for usb0
  USB_Pin_Clk_Init();
// initialize call back structures
  memset((void*)&usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
  usb_param.usb_reg_base = LPC_USB_BASE;
  usb_param.mem_base = 0x20004000;
  usb_param.mem_size = 0x0500;
  usb_param.max_num_ep = 3;
// Initialize CDC parameters
  memset((void*)&cdc_param, 0, sizeof(USBD_CDC_INIT_PARAM_T));
  memset((void*)&g_vCOM, 0, sizeof(VCOM_DATA_T));
// user defined functions
#if defined(UART_BRIDGE)
  cdc_param.SetLineCode = VCOM_SetLineCode;
  usb_param.USB_SOF_Event = VCOM_SOF_Event;
#endif
  cdc_param.SendBreak = VCOM_SendBreak;
// Initialize Descriptor pointers
  memset((void*)&desc, 0, sizeof(USB_CORE_DESCS_T));
  desc.device_desc = (uint8_t *)&VCOM_DeviceDescriptor[0];
  desc.string_desc = (uint8_t *)&VCOM_StringDescriptor[0];
  desc.full_speed_desc = (uint8_t *)&VCOM_ConfigDescriptor[0];
  desc.high_speed_desc = (uint8_t *)&VCOM_ConfigDescriptor[0];
// USB Initialization
  ret = pUsbApi->hw->Init(&hUsb, &desc, &usb_param);
  if (ret == LPC_OK)
  {
// init CDC params
    cdc_param.mem_base = 0x20004500;
    cdc_param.mem_size = 0x0300;
    cdc_param.cif_intf_desc = (uint8_t *)&VCOM_ConfigDescriptor[USB_CONFIGUARTION_DESC_SIZE];
    cdc_param.dif_intf_desc = (uint8_t *)&VCOM_ConfigDescriptor[USB_CONFIGUARTION_DESC_SIZE + \
                               USB_INTERFACE_DESC_SIZE + 0x0013 + USB_ENDPOINT_DESC_SIZE ];
    ret = pUsbApi->cdc->init(hUsb, &cdc_param, &hCdc);
    if (ret == LPC_OK)
    {
// store USB handle
      g_vCOM.hUsb = hUsb;
      g_vCOM.hCdc = hCdc;
      g_vCOM.send_fn = VCOM_USB_Send;
// allocate transfer buffers
      g_vCOM.rxBuf = (uint8_t*)(cdc_param.mem_base + (0 * USB_HS_MAX_BULK_PACKET));
      g_vCOM.txBuf = (uint8_t*)(cdc_param.mem_base + (1 * USB_HS_MAX_BULK_PACKET));
      cdc_param.mem_size -= (4 * USB_HS_MAX_BULK_PACKET);
// register end_point interrupt handler
      ep_indx = (((USB_CDC_EP_BULK_IN & 0x0F) << 1) + 1);
      ret = pUsbApi->core->RegisterEpHandler (hUsb, ep_indx, VCOM_Bulk_In_Hdlr, &g_vCOM);
      if (ret == LPC_OK)
      {
// register end_point interrupt handler
        ep_indx = ((USB_CDC_EP_BULK_OUT & 0x0F) << 1);
        ret = pUsbApi->core->RegisterEpHandler (hUsb, ep_indx, VCOM_Bulk_Out_Hdlr, &g_vCOM);
        if (ret == LPC_OK)
        {
// enable IRQ
          NVIC_EnableIRQ(USB_IRQn); //  enable USB0 interrrupts
#if defined(UART_BRIDGE)
          g_vCOM.send_fn = VCOM_UART_Send;
// Initialize UART for bridge
          UART_Bridge_Init(&g_vCOM, 0);
// enable IRQ
          NVIC_EnableIRQ(UART_IRQn); //  enable Uart interrrupt
#endif
// USB Connect
          pUsbApi->hw->Connect(hUsb, 1);
        }
      }
    }
  }
}
//---

// IRQ Handlers---
void USB_IRQHandler(void)
{
	uint32_t *addr = (uint32_t *) LPC_USB->EPLISTSTART;
// WORKAROUND for artf32289 ROM driver BUG:
//	As part of USB specification the device should respond
//  with STALL condition for any unsupported setup packet. The host will send
//  new setup packet/request on seeing STALL condition for EP0 instead of sending
//  a clear STALL request. Current driver in ROM doesn't clear the STALL
//  condition on new setup packet which should be fixed.
	if (LPC_USB->DEVCMDSTAT & _BIT(8))		// if setup packet is received
	{
		addr[0] &= ~(_BIT(29));	// clear EP0_OUT stall
	    addr[2] &= ~(_BIT(29));	// clear EP0_IN stall
	}
	pUsbApi->hw->ISR(g_vCOM.hUsb);
}

void UART_IRQHandler (void)
{
  VCOM_DATA_T* pVcom = &g_vCOM;
  uint8_t intId, lsr;
  uint16_t serial_state = 0;
  intId = (LPC_USART->IIR >> 1) & 0x07; // check bit 1~3, interrupt identification
  switch (intId)
  {
    case IIR_RLS:	// Receive Line Status
      lsr = LPC_USART->LSR;	// Read LSR will clear the interrupt
// There are errors or break interrupt update serial_state
      if (lsr & LSR_OE)
      {
          serial_state |= CDC_SERIAL_STATE_OVERRUN;
      }
      if (lsr & LSR_PE)
      {
          serial_state |= CDC_SERIAL_STATE_PARITY;
      }
      if (lsr & LSR_FE)
      {
          serial_state |= CDC_SERIAL_STATE_FRAMING;
      }
      if (lsr & LSR_BI)
      {
          serial_state |= CDC_SERIAL_STATE_BREAK;
      }
      pUsbApi->cdc->SendNotification (pVcom->hCdc, CDC_NOTIFICATION_SERIAL_STATE, serial_state);
      break;
    case IIR_CTI: 	// Character timeout indicator
      uart_read(pVcom);	// send packet to USB, read chars until FIFO empty
      break;
    case IIR_RDA:  // Receive Data Available
      uart_read(pVcom);	// 4 chars are available in FIFO
uint8_t *pbuf = pVcom->rxBuf;
     xData = *pbuf;
     VCOM_USB_Send(pVcom);
      break;
    case  IIR_THRE:	// THRE, transmit holding register empty
      if (pVcom->rxlen)
      {
          uart_write(pVcom);	// transmit
      }
      break;
  }
  return;
}
//---

// Functions---
void VCOM_USB_Send(VCOM_DATA_T* pVcom)
{
  // when data received, send it back
	pVcom->txlen -= pUsbApi->hw->WriteEP (pVcom->hUsb, USB_CDC_EP_BULK_IN, pVcom->txBuf, pVcom->txlen);
}

void VCOM_UART_Send(VCOM_DATA_T* pVcom)
{
	// when data received on USB, send it to UART
	uart_write(pVcom);
	LOW(1, 19);
}

ErrorCode_t VCOM_SetLineCode (USBD_HANDLE_T hCDC, CDC_LINE_CODING* line_coding)
{
  VCOM_DATA_T* pVcom = &g_vCOM;
  // baud rate change, reset buffers
  pVcom->ser_pos = 0;
  pVcom->rxlen = pVcom->txlen = 0;
//  UART_Bridge_Init(pVcom, line_coding);
  return LPC_OK;
}

ErrorCode_t VCOM_SOF_Event(USBD_HANDLE_T hUsb)
{
  VCOM_DATA_T* pVcom = &g_vCOM;
  uint8_t lcr;
  uint32_t diff = pVcom->sof_counter - pVcom->last_ser_rx;
  pVcom->sof_counter++;
  if (pVcom->break_time)
  {
    pVcom->break_time--;
    if (pVcom->break_time == 0)
    {
      lcr = LPC_USART->LCR;
      if (lcr & (1 << 6))
      {
        lcr &= ~(1 << 6);
        LPC_USART->LCR = lcr;
      }
    }
  }
  if ( pVcom->last_ser_rx && (diff > 5))
  {
	  VCOM_USB_Send(pVcom);
  }
  return LPC_OK;
}

ErrorCode_t VCOM_SendBreak (USBD_HANDLE_T hCDC, uint16_t mstime)
{
  VCOM_DATA_T* pVcom = &g_vCOM;
  uint8_t lcr = LPC_USART->LCR;
  if (mstime)
  {
    lcr |= (1 << 6);
  }
  else
  {
    lcr &= ~(1 << 6);
  }
  pVcom->break_time = mstime;
  LPC_USART->LCR = lcr;
  return LPC_OK;
}

ErrorCode_t VCOM_Bulk_In_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event)
{
// VCOM_DATA_T* pVcom = (VCOM_DATA_T*) data;
// Not needed as WriteEP() is called in VCOM_usb_send() immediately.
// if (event == USB_EVT_IN)
//	{
//  }
  return LPC_OK;
}

ErrorCode_t VCOM_Bulk_Out_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event)
{
  VCOM_DATA_T* pVcom = (VCOM_DATA_T*) data;
  switch (event)
  {
    case
	USB_EVT_OUT:
      if (pVcom->rxlen == 0)
      {
        pVcom->rxlen = pUsbApi->hw->ReadEP(hUsb, USB_CDC_EP_BULK_OUT, pVcom->rxBuf);
        pVcom->send_fn(pVcom);
      }
      else	// indicate bridge write buffer pending in USB buffer
      {
        pVcom->usbrx_pend = 1;
      }
      break;
    default: 
      break;
  }
  return LPC_OK;
}
/**********************************************************************
 **                            End Of File
 **********************************************************************/
