################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Driver_Lib/src/cdc_desc.c \
../Driver_Lib/src/clkconfig.c \
../Driver_Lib/src/gpio.c \
../Driver_Lib/src/i2c.c \
../Driver_Lib/src/nmi.c \
../Driver_Lib/src/ssp.c \
../Driver_Lib/src/timer16.c \
../Driver_Lib/src/timer32.c \
../Driver_Lib/src/uart.c 

OBJS += \
./Driver_Lib/src/cdc_desc.o \
./Driver_Lib/src/clkconfig.o \
./Driver_Lib/src/gpio.o \
./Driver_Lib/src/i2c.o \
./Driver_Lib/src/nmi.o \
./Driver_Lib/src/ssp.o \
./Driver_Lib/src/timer16.o \
./Driver_Lib/src/timer32.o \
./Driver_Lib/src/uart.o 

C_DEPS += \
./Driver_Lib/src/cdc_desc.d \
./Driver_Lib/src/clkconfig.d \
./Driver_Lib/src/gpio.d \
./Driver_Lib/src/i2c.d \
./Driver_Lib/src/nmi.d \
./Driver_Lib/src/ssp.d \
./Driver_Lib/src/timer16.d \
./Driver_Lib/src/timer32.d \
./Driver_Lib/src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Driver_Lib/src/%.o: ../Driver_Lib/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=c99 -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_ROMDIVIDE -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Hidemichi_Gotou\Documents\LPCXpresso_8.2.2_650\workspace\LPC11U37_Controller\inc" -I"C:\Users\Hidemichi_Gotou\Documents\LPCXpresso_8.2.2_650\workspace\CMSIS_CORE_LPC11Uxx\inc" -I"C:\Users\Hidemichi_Gotou\Documents\LPCXpresso_8.2.2_650\workspace\LPC11U37_Controller\Driver_Lib\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


