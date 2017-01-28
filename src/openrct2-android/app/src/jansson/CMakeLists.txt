# add_library(app-glue STATIC
#            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

include_directories(${JANSSON_ROOT}/android)
include_directories(${JANSSON_ROOT}/src)
file(GLOB_RECURSE JANSSON_SOURCES "${JANSSON_ROOT}/src/*.c")

add_definitions(-DHAVE_STDINT_H=1)
add_library(jansson SHARED ${JANSSON_SOURCES})

# copy out the lib binary and remove generated files
#
# LOCAL_MODULE_TAGS := optional
# LOCAL_SHARED_LIBRARIES := libc
# LOCAL_CFLAGS += -O3 -DHAVE_STDINT_H=1
#
# LOCAL_MODULE:= libjansson
#
# include $(BUILD_SHARED_LIBRARY)
