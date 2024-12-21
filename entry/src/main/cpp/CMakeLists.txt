# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(arkui_cpp_demo)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(OPENCV_LIB_PATH ${NATIVERENDER_ROOT_PATH}/../../../libs/opencv-mobile-2.4.13.7/${OHOS_ARCH}/lib)
set(OPENCV_INCLUDE_PATH ${NATIVERENDER_ROOT_PATH}/../../../libs/opencv-mobile-2.4.13.7/${OHOS_ARCH}/include)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(
    ${NATIVERENDER_ROOT_PATH}
    ${NATIVERENDER_ROOT_PATH}/include
    ${OPENCV_INCLUDE_PATH}
)

add_library(entry SHARED
    napi_init.cpp
    common/common.cpp
)

target_link_libraries(entry PUBLIC
    libace_napi.z.so
    librawfile.z.so
    libhilog_ndk.z.so
)

target_link_libraries(entry PUBLIC
    ${OPENCV_LIB_PATH}/libopencv_core.a
    ${OPENCV_LIB_PATH}/libopencv_highgui.a
    ${OPENCV_LIB_PATH}/libopencv_imgproc.a
    ${OPENCV_LIB_PATH}/libopencv_photo.a
    ${OPENCV_LIB_PATH}/libopencv_video.a
)

# 添加编译定义，禁用 highgui 的显示功能
target_compile_definitions(entry PRIVATE
    HAVE_NO_WINDOW_SUPPORT
)