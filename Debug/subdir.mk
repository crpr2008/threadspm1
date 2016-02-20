################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example_threads.c \
../testhw.c \
../testhw_sync.c \
../thr.c \
../thread123.c 

OBJS += \
./example_threads.o \
./testhw.o \
./testhw_sync.o \
./thr.o \
./thread123.o 

C_DEPS += \
./example_threads.d \
./testhw.d \
./testhw_sync.d \
./thr.d \
./thread123.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


