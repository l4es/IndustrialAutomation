################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/board.c \
../src/board_sysinit.c \
../src/lpc_nandflash_k9f1g.c \
../src/lpc_norflash_sst39vf320.c \
../src/lpc_phy_smsc87x0.c \
../src/mem_tests.c \
../src/uda1380.c 

OBJS += \
./src/board.o \
./src/board_sysinit.o \
./src/lpc_nandflash_k9f1g.o \
./src/lpc_norflash_sst39vf320.o \
./src/lpc_phy_smsc87x0.o \
./src/mem_tests.o \
./src/uda1380.o 

C_DEPS += \
./src/board.d \
./src/board_sysinit.d \
./src/lpc_nandflash_k9f1g.d \
./src/lpc_norflash_sst39vf320.d \
./src/lpc_phy_smsc87x0.d \
./src/mem_tests.d \
./src/uda1380.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"D:\dev\_draft\CANopen\lpc_chip_177x_8x\inc" -I"D:\dev\_draft\CANopen\lpc_board_ea_devkit_1788\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


