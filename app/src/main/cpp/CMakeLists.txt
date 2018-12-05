# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(PATH_TO_AV_ENGINE ${PATH_TO_MEDIACORE}/src/main/cpp)
set(PATH_TO_THIRDPARTY ${PATH_TO_AV_ENGINE}/thirdparty)
set(PATH_TO_PRE_BUILT ${PATH_TO_THIRDPARTY}/prebuilt)

message(${PATH_TO_AV_ENGINE})
message(${ANDROID_ABI})

include_directories(BEFORE ${PATH_TO_THIRDPARTY}/include/)
include_directories(${PATH_TO_AV_ENGINE}/)
include_directories(${PATH_TO_AV_ENGINE}/encoder/)
include_directories(${PATH_TO_AV_ENGINE}/decoder/)

file(GLOB FILES_LIB_COMMON "${PATH_TO_AV_ENGINE}/*.cpp")
file(GLOB FILES_LIB_ENCODER "${PATH_TO_AV_ENGINE}/encoder/*.cpp")
file(GLOB FILES_LIB_DECODER "${PATH_TO_AV_ENGINE}/decoder/*.cpp")

add_library(av-engine STATIC
        ${FILES_LIB_COMMON}
        ${FILES_LIB_DECODER}
        ${FILES_LIB_ENCODER}
        )

target_link_libraries(av-engine
                       # 引入系统的动态库
                       log
                       # 引入ffmpeg相关静态库
                       ${PATH_TO_PRE_BUILT}/libfdk-aac.a
                       )