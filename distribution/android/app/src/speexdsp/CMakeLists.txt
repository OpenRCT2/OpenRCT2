cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)

set(SPEEXDSP_LIB_ROOT "${SPEEXDSP_ROOT}/libspeexdsp")

SET(SPEEXDSP_LIB_SRCS
   "${SPEEXDSP_LIB_ROOT}/buffer.c"
   "${SPEEXDSP_LIB_ROOT}/fftwrap.c"
   "${SPEEXDSP_LIB_ROOT}/filterbank.c"
   "${SPEEXDSP_LIB_ROOT}/jitter.c"
   "${SPEEXDSP_LIB_ROOT}/kiss_fft.c"
   "${SPEEXDSP_LIB_ROOT}/kiss_fftr.c"
   "${SPEEXDSP_LIB_ROOT}/mdf.c"
   "${SPEEXDSP_LIB_ROOT}/preprocess.c"
   "${SPEEXDSP_LIB_ROOT}/resample.c"
   "${SPEEXDSP_LIB_ROOT}/scal.c"
   "${SPEEXDSP_LIB_ROOT}/smallft.c"
)

add_definitions(-DFIXED_POINT -DUSE_KISS_FFT -DHAVE_STDINT_H)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DEXPORT='' -UHAVE_CONFIG_H")
include_directories("${SPEEXDSP_ROOT}/include" ${SPEEXDSP_LIB_ROOT} "${CMAKE_SOURCE_DIR}/../speexdsp")
add_library(speexdsp SHARED ${SPEEXDSP_LIB_SRCS})
target_link_libraries(speexdsp m)
