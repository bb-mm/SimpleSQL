################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/pf_buffermgr.cc \
../src/pf_error.cc \
../src/pf_filehandle.cc \
../src/pf_hashtable.cc \
../src/pf_manager.cc \
../src/pf_pagehandle.cc \
../src/pf_statistics.cc \
../src/pf_test1.cc \
../src/pf_test2.cc \
../src/pf_test3.cc \
../src/rm_error.cc \
../src/rm_filehandle.cc \
../src/rm_filescan.cc \
../src/rm_manager.cc \
../src/rm_record.cc \
../src/rm_rid.cc \
../src/rm_test.cc \
../src/statistics.cc 

OBJS += \
./src/pf_buffermgr.o \
./src/pf_error.o \
./src/pf_filehandle.o \
./src/pf_hashtable.o \
./src/pf_manager.o \
./src/pf_pagehandle.o \
./src/pf_statistics.o \
./src/pf_test1.o \
./src/pf_test2.o \
./src/pf_test3.o \
./src/rm_error.o \
./src/rm_filehandle.o \
./src/rm_filescan.o \
./src/rm_manager.o \
./src/rm_record.o \
./src/rm_rid.o \
./src/rm_test.o \
./src/statistics.o 

CC_DEPS += \
./src/pf_buffermgr.d \
./src/pf_error.d \
./src/pf_filehandle.d \
./src/pf_hashtable.d \
./src/pf_manager.d \
./src/pf_pagehandle.d \
./src/pf_statistics.d \
./src/pf_test1.d \
./src/pf_test2.d \
./src/pf_test3.d \
./src/rm_error.d \
./src/rm_filehandle.d \
./src/rm_filescan.d \
./src/rm_manager.d \
./src/rm_record.d \
./src/rm_rid.d \
./src/rm_test.d \
./src/statistics.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


