# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(
	PATH_TO_MEDIACORE
	CACHE STRING ""
)
set(PATH_TO_NATIVE ${PATH_TO_MEDIACORE}/src/main/cpp)
set(PATH_TO_JNI_LAYER ${PATH_TO_MEDIACORE}/src/main/jni)
set(PATH_TO_THIRDPARTY ${PATH_TO_NATIVE}/thirdparty)
set(PATH_TO_PRE_BUILT ${PATH_TO_THIRDPARTY}/prebuilt)

add_definitions("
-DGL_GLEXT_PROTOTYPES
-DEGL_EGLEXT_PROTOTYPES
")

message(${PATH_TO_JNI_LAYER})

ADD_SUBDIRECTORY(src/main/cpp)

include_directories(BEFORE ${PATH_TO_THIRDPARTY}/include/)
include_directories(${PATH_TO_NATIVE}/)
include_directories(${PATH_TO_NATIVE}/decoder/)
include_directories(${PATH_TO_NATIVE}/encoder/)

file(GLOB FILES_JNI_LAYER "${PATH_TO_JNI_LAYER}/*.cpp")

add_library(native-lib SHARED
        ${FILES_JNI_LAYER}
        )

target_link_libraries(native-lib
                       # 引入系统的动态库
                       log
                       av-engine
                       # 引入fdk-aac相关静态库
                       ${PATH_TO_PRE_BUILT}/libfdk-aac.a
                       )