
#include <jni.h>
#include <android/log.h>
#include "libmediasoupclient/include/mediasoupclient.hpp"
#include "libmediasoupclient/jni/jni_helper.hpp"


JOWW(void, MediaSoupClient_initialize)(JNIEnv *env, jclass cls) {

}


JOWW(void, MediaSoupClient_cleanup)(JNIEnv *env, jclass cls) {
 
}


JOWW(jstring, MediaSoupClient_version)(JNIEnv *env, jclass cls) {
    std::string version = mediasoupclient::Version();
    jstring result = env->NewStringUTF(version.c_str()); 
    return result;     
}
