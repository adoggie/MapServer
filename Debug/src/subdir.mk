################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/base.cpp \
../src/base64.cpp \
../src/bfod.cpp \
../src/dbglog.cpp \
../src/ftr_mpoint.cpp \
../src/geo.cpp \
../src/geo_help.cpp \
../src/geo_poi.cpp \
../src/geo_polygon.cpp \
../src/geo_road.cpp \
../src/layer.cpp \
../src/map.cpp \
../src/matrix.cpp \
../src/object.cpp \
../src/persist.cpp \
../src/render.cpp \
../src/resource.cpp \
../src/server.cpp \
../src/stdafx.cpp \
../src/util.cpp \
../src/view.cpp 

OBJS += \
./src/base.o \
./src/base64.o \
./src/bfod.o \
./src/dbglog.o \
./src/ftr_mpoint.o \
./src/geo.o \
./src/geo_help.o \
./src/geo_poi.o \
./src/geo_polygon.o \
./src/geo_road.o \
./src/layer.o \
./src/map.o \
./src/matrix.o \
./src/object.o \
./src/persist.o \
./src/render.o \
./src/resource.o \
./src/server.o \
./src/stdafx.o \
./src/util.o \
./src/view.o 

CPP_DEPS += \
./src/base.d \
./src/base64.d \
./src/bfod.d \
./src/dbglog.d \
./src/ftr_mpoint.d \
./src/geo.d \
./src/geo_help.d \
./src/geo_poi.d \
./src/geo_polygon.d \
./src/geo_road.d \
./src/layer.d \
./src/map.d \
./src/matrix.d \
./src/object.d \
./src/persist.d \
./src/render.d \
./src/resource.d \
./src/server.d \
./src/stdafx.d \
./src/util.d \
./src/view.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/cairomm-1.0 -I"/home/projects/newgis.2013/swGis/src/tce/cpp" -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


