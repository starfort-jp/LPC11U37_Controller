################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/aeabi_romdiv_patch.s 

C_SRCS += \
../src/cr_startup_lpc11uxx.c \
../src/crp.c \
../src/i2clcd.c \
../src/main.c \
../src/systick.c \
../src/xprintf.c 

OBJS += \
./src/aeabi_romdiv_patch.o \
./src/cr_startup_lpc11uxx.o \
./src/crp.o \
./src/i2clcd.o \
./src/main.o \
./src/systick.o \
./src/xprintf.o 

C_DEPS += \
./src/cr_startup_lpc11uxx.d \
./src/crp.d \
./src/i2clcd.d \
./src/main.d \
./src/systick.d \
./src/xprintf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DNDEBUG -D__CODE_RED -DCORE_M0 -D__USE_ROMDIVIDE -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Hidemichi_Gotou\Documents\LPCXpresso_8.2.2_650\workspace\LPC11U37_Controller\inc" -I"C:\Users\Hidemichi_Gotou\Documents\LPCXpresso_8.2.2_650\workspace\CMSIS_CORE_LPC11Uxx\inc" -mcpu=cortex-m0 -mthumb -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=c99 -DNDEBUG -D__CODE_RED -DCORE_M0 -D__USE_ROMDIVIDE -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Hidemichi_Gotou\Documents\LPCXpresso_8.2.2_650\workspace\LPC11U37_Controller\inc" -I"C:\Users\Hidemichi_Gotou\Documents\LPCXpresso_8.2.2_650\workspace\CMSIS_CORE_LPC11Uxx\inc" -I"C:\Users\Hidemichi_Gotou\Documents\LPCXpresso_8.2.2_650\workspace\LPC11U37_Controller\Driver_Lib\inc" -Os -fno-common -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


