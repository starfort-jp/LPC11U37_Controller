/***********************************************************************
*
* usb_rom_func.h : USB device ROM Stack module header
*
**********************************************************************/

// USB Communication Device Class User module
extern char xData;

struct VCOM_DATA;
// Types---
typedef void (*VCOM_SEND_T) (struct VCOM_DATA* pVcom);
typedef struct VCOM_DATA {
  USBD_HANDLE_T hUsb;
  USBD_HANDLE_T hCdc;
  uint8_t* rxBuf;
  uint8_t* txBuf;
  volatile uint8_t ser_pos;
  volatile uint16_t rxlen;
  volatile uint16_t txlen;
  VCOM_SEND_T send_fn;
  volatile uint32_t sof_counter;
  volatile uint32_t last_ser_rx;
  volatile uint16_t break_time;
  volatile uint16_t usbrx_pend;
} VCOM_DATA_T;
//---

// Routines---
// Initialization
extern void UART_Bridge_Init(VCOM_DATA_T* pVcom, CDC_LINE_CODING* line_coding);
extern void USB_Pin_Clk_Init(void);
extern void USB_CDC_Init (void);

// IRQ Handlers
extern void UART_IRQHandler (void);
extern void USB_IRQHandler(void);

// Functions
extern void VCOM_USB_Send(VCOM_DATA_T* pVcom);
extern void VCOM_UART_Send(VCOM_DATA_T* pVcom);
extern ErrorCode_t VCOM_SetLineCode (USBD_HANDLE_T hCDC, CDC_LINE_CODING* line_coding);
extern ErrorCode_t VCOM_SOF_Event(USBD_HANDLE_T hUsb);
extern ErrorCode_t VCOM_SendBreak (USBD_HANDLE_T hCDC, uint16_t mstime);
extern ErrorCode_t VCOM_Bulk_In_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event) ;
extern ErrorCode_t VCOM_Bulk_Out_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event) ;
//---
