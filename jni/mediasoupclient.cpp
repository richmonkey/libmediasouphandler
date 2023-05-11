
#include <jni.h>
#include <android/log.h>
#include <android/looper.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h> /* INT_MAX, PATH_MAX */
#include <sys/uio.h> /* writev */
#include <sys/ioctl.h>
#include <errno.h>
#include <strings.h>
#include <netdb.h>
#include <assert.h>
#include "libmediasoupclient/include/Device.hpp"
#include "libmediasoupclient/include/mediasoupclient.hpp"
#include "libmediasoupclient/jni/jni_helper.hpp"


JOWW(void, MediaSoupClient_initialize)(JNIEnv *env, jobject object) {
    mediasoupclient::Initialize();
}


JOWW(void, MediaSoupClient_cleanup)(JNIEnv *env, jobject object) {
    mediasoupclient::Cleanup();    
}


JOWW(jstring, MediaSoupClient_version)(JNIEnv *env, jobject object) {
    std::string version = mediasoupclient::Version();
    jstring result = env->NewStringUTF(version.c_str()); 
    return result;     
}
