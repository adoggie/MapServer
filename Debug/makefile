################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/tileserver/subdir.mk
-include src/tiles/subdir.mk
-include src/swbox/subdir.mk
-include src/sld/subdir.mk
-include src/memheap/subdir.mk
-include src/algo/subdir.mk
-include src/subdir.mk
-include cpp/tce/websocket/subdir.mk
-include cpp/tce/utils/subdir.mk
-include cpp/tce/tinyxml/subdir.mk
-include cpp/tce/socket/subdir.mk
-include cpp/tce/qpid/subdir.mk
-include cpp/tce/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: tileserver_cario

# Tool invocations
tileserver_cario: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "tileserver_cario" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) tileserver_cario
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
