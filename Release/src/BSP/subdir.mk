################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BSP/IOexp.c \
../src/BSP/lcdBSP.c \
../src/BSP/spi.c \
../src/BSP/uart.c 

OBJS += \
./src/BSP/IOexp.o \
./src/BSP/lcdBSP.o \
./src/BSP/spi.o \
./src/BSP/uart.o 

C_DEPS += \
./src/BSP/IOexp.d \
./src/BSP/lcdBSP.d \
./src/BSP/spi.d \
./src/BSP/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/BSP/%.o: ../src/BSP/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


