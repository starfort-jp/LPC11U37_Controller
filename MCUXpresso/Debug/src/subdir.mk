################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/aeabi_romdiv_patch.s 

C_SRCS += \
../src/LPC11U37_Controller.c \
../src/cdc_desc.c \
../src/clkconfig.c \
../src/cr_startup_lpc11uxx.c \
../src/crp.c \
../src/gpio.c \
../src/i2c.c \
../src/i2clcd.c \
../src/nmi.c \
../src/ssp.c \
../src/systick.c \
../src/timer16.c \
../src/timer32.c \
../src/uart.c \
../src/usb_rom_func.c \
../src/xprintf.c 

OBJS += \
./src/LPC11U37_Controller.o \
./src/aeabi_romdiv_patch.o \
./src/cdc_desc.o \
./src/clkconfig.o \
./src/cr_startup_lpc11uxx.o \
./src/crp.o \
./src/gpio.o \
./src/i2c.o \
./src/i2clcd.o \
./src/nmi.o \
./src/ssp.o \
./src/systick.o \
./src/timer16.o \
./src/timer32.o \
./src/uart.o \
./src/usb_rom_func.o \
./src/xprintf.o 

C_DEPS += \
./src/LPC11U37_Controller.d \
./src/cdc_desc.d \
./src/clkconfig.d \
./src/cr_startup_lpc11uxx.d \
./src/crp.d \
./src/gpio.d \
./src/i2c.d \
./src/i2clcd.d \
./src/nmi.d \
./src/ssp.d \
./src/systick.d \
./src/timer16.d \
./src/timer32.d \
./src/uart.d \
./src/usb_rom_func.d \
./src/xprintf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_ROMDIVIDE -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Hidemichi_Gotou\Documents\MCUXpressoIDE_10.0.0_344\workspace\LPC11U37_Controller\inc" -I"C:\Users\Hidemichi_Gotou\Documents\MCUXpressoIDE_10.0.0_344\workspace\CMSIS_CORE_LPC11Uxx\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_ROMDIVIDE -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Hidemichi_Gotou\Documents\MCUXpressoIDE_10.0.0_344\workspace\LPC11U37_Controller\inc" -I"C:\Users\Hidemichi_Gotou\Documents\MCUXpressoIDE_10.0.0_344\workspace\CMSIS_CORE_LPC11Uxx\inc" -g3 -mcpu=cortex-m0 -mthumb -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


