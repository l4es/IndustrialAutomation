################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/can.c \
../example/src/cr_startup_lpc177x_8x.c \
../example/src/sysinit.c 

OBJS += \
./example/src/can.o \
./example/src/cr_startup_lpc177x_8x.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/can.d \
./example/src/cr_startup_lpc177x_8x.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"D:\dev\_draft\CANopen\lpc_chip_177x_8x\inc" -I"D:\dev\_draft\CANopen\lpc_board_ea_devkit_1788\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


