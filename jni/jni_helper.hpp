#ifndef MSC_JNI_HELPER_HPP
#define MSC_JNI_HELPER_HPP

#define JOWW(rettype, name)                                             \
  extern "C" JNIEXPORT rettype JNICALL Java_org_mediasoup_##name



#define DEBUG 1
#if DEBUG
#define  LOG(fmt, ...)  __android_log_print(ANDROID_LOG_INFO,"beetle",\
                                            "file:%s, line:%d " fmt, __FILE__, __LINE__, \
                                              ##__VA_ARGS__)
#else
#define  LOG(...)  do {} while (0)
#endif



#endif
