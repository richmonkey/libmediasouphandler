
#include <jni.h>
#include <android/log.h>
#include "libmediasoupclient/include/mediasoupclient.hpp"
#include "libmediasoupclient/include/Logger.hpp"
#include "libmediasoupclient/jni/jni_helper.hpp"

class AndroidLog : public mediasoupclient::Logger::LogHandlerInterface {
    void OnLog(mediasoupclient::Logger::LogLevel level, char* payload, size_t len) {
        LOG_NO_FILE_LINE("%s", payload);
    }
};

JOWW(void, MediaSoupClient_initialize)(JNIEnv *env, jclass cls) {
    mediasoupclient::Logger::SetHandler(new AndroidLog());
    mediasoupclient::Logger::SetLogLevel(mediasoupclient::Logger::LogLevel::LOG_TRACE);
}


JOWW(void, MediaSoupClient_cleanup)(JNIEnv *env, jclass cls) {
 
}


JOWW(jstring, MediaSoupClient_version)(JNIEnv *env, jclass cls) {
    std::string version = mediasoupclient::Version();
    jstring result = env->NewStringUTF(version.c_str()); 
    return result;     
}
