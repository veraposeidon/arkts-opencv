# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(arkui_cpp_demo)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(entry SHARED napi_init.cpp)
target_link_libraries(entry PUBLIC libace_napi.z.so)

#target_link_libraries(entry PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/opencv-mobile-2.4.13.7/${OHOS_ARCH}/lib/libopencv_core.a
#    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/opencv-mobile-2.4.13.7/${OHOS_ARCH}/lib/libopencv_highgui.a
#    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/opencv-mobile-2.4.13.7/${OHOS_ARCH}/lib/libopencv_imgproc.a
#    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/opencv-mobile-2.4.13.7/${OHOS_ARCH}/lib/libopencv_photo.a
#    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/opencv-mobile-2.4.13.7/${OHOS_ARCH}/lib/libopencv_video.a
#)
#
#target_include_directories(entry PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/opencv-mobile-2.4.13.7/${OHOS_ARCH}/include
#)
