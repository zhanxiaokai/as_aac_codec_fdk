#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring

JNICALL
Java_media_ushow_aac_1codec_1fdk_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ AAC Codec For libfdk-aac Test";
    return env->NewStringUTF(hello.c_str());
}