#include <jni.h>
#include <android/log.h>
// #include <android/looper.h>

#include "api/peer_connection_interface.h"
#include "sdk/android/native_api/jni/java_types.h"
#include "sdk/android/src/jni/pc/peer_connection.h"
#include "sdk/android/src/jni/pc/owned_factory_and_threads.h"
#include "libmediasoupclient/include/Device.hpp"
#include "libmediasoupclient/include/PeerConnection.hpp"
#include "libmediasoupclient/include/Transport.hpp"
#include "libmediasoupclient/jni/jni_helper.hpp"
#include "libmediasoupclient/jni/TransportJni.hpp"
#include <json.hpp>

static jlong getNativeFactory(JNIEnv *env, jobject j_factory) {
    jclass cls = env->GetObjectClass(j_factory);
    jfieldID field = env->GetFieldID(cls, "nativeFactory", "J");
    return env->GetLongField(j_factory, field);
}

static webrtc::PeerConnectionFactoryInterface* PeerConnectionFactoryFromJava(jlong j_p) {
    return reinterpret_cast<webrtc::jni::OwnedFactoryAndThreads*>(j_p)->factory();
}

static jobject Java_SendTransport_Constructor(JNIEnv *env, jlong nativeTransport) {
    jclass cls = env->FindClass("org/mediasoup/SendTransport");
    auto constructor = env->GetMethodID(cls, "<init>", "(J)V");
    auto object = env->NewObject(cls, constructor, nativeTransport);
    return object;
}

static jobject Java_RecvTransport_Constructor(JNIEnv *env, jlong nativeTransport) {
    jclass cls = env->FindClass("org/mediasoup/RecvTransport");
    auto constructor = env->GetMethodID(cls, "<init>", "(J)V");
    auto object = env->NewObject(cls, constructor, nativeTransport);
    return object;
}


JOWW(jlong, Device_nativeCreateDevice)(JNIEnv *env, jobject object) {
    mediasoupclient::Device *device = new mediasoupclient::Device();
    return webrtc::NativeToJavaPointer(device);
}

JOWW(void, Device_nativeLoad)(JNIEnv *env,
                              jobject object,
                              jlong nativeDevice,
                              jstring routerRtpCapabilities,
                              jobject rtcConfig,
                              jobject j_factory) {
    LOG("test");
    const webrtc::JavaParamRef<jobject>& j_rtc_config = webrtc::JavaParamRef<jobject>(env, rtcConfig);
    
    webrtc::PeerConnectionInterface::RTCConfiguration rtc_config;
    
    webrtc::jni::JavaToNativeRTCConfiguration(env, j_rtc_config, &rtc_config);

    const char *router_rtpcap = env->GetStringUTFChars(routerRtpCapabilities, NULL);

    auto cap = nlohmann::json::parse(router_rtpcap);

    jlong factory = getNativeFactory(env, j_factory);
    webrtc::PeerConnectionFactoryInterface *factory_interface = PeerConnectionFactoryFromJava(factory);
    
    mediasoupclient::PeerConnection::Options options;
    options.config = rtc_config;
    options.factory = factory_interface;

    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);

    device->Load(cap, &options);
    
    env->ReleaseStringUTFChars(routerRtpCapabilities, router_rtpcap);
}



JOWW(jboolean, Device_nativeIsLoaded)(JNIEnv *env, jobject object,
                                      jlong nativeDevice) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);    
    return device->IsLoaded() ? JNI_TRUE : JNI_FALSE;
}



JOWW(void, Device_nativeRelease)(JNIEnv *env, jobject object,
                                      jlong nativeDevice) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);    
    delete device;
}



JOWW(jstring, Device_nativeGetRtpCapabilities)(JNIEnv *env, jobject object,
                                      jlong nativeDevice) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);    
    auto rtpCap = device->GetRtpCapabilities();
    auto s_rtpCap = rtpCap.dump();
    auto result = webrtc::NativeToJavaString(env, s_rtpCap);
    return result.Release();
}



JOWW(jboolean, Device_nativeCanProduce)(JNIEnv *env, jobject object,
                                       jlong nativeDevice, jstring kind) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);

    auto k = webrtc::JavaToStdString(env, kind);
    
    auto result = device->CanProduce(k);

    return result ? JNI_TRUE : JNI_FALSE;
}


JOWW(jobject, Device_nativeCreateSendTransport)(JNIEnv *env, jobject object,
                                              jlong nativeDevice,
                                              jobject j_transport,
                                              jobject j_listener,
                                              jstring j_id,
                                              jstring j_iceParameters,
                                              jstring j_iceCandidates,
                                              jstring j_dtlsParameters) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);
    auto id = webrtc::JavaToStdString(env, j_id);
    auto s_iceParameters = webrtc::JavaToStdString(env, j_iceParameters);
    auto s_iceCandidates = webrtc::JavaToStdString(env, j_iceCandidates);
    auto s_dtlsParameters = webrtc::JavaToStdString(env, j_dtlsParameters);

    auto iceParameters = nlohmann::json::parse(s_iceParameters);
    auto iceCandidates = nlohmann::json::parse(s_iceCandidates);
    auto dtlsParameters = nlohmann::json::parse(s_dtlsParameters);

    auto listener = new TransportObserverJni();

    auto transport = device->CreateSendTransport(
      listener,
      id,
      iceParameters,
      iceCandidates,
      dtlsParameters);
    
    return Java_SendTransport_Constructor(env, webrtc::NativeToJavaPointer(transport));
}

JOWW(jobject, Device_nativeCreateRecvTransport)(JNIEnv *env, jobject object,
                                              jlong nativeDevice,
                                              jobject j_transport,
                                              jobject j_listener,
                                              jstring j_id,
                                              jstring j_iceParameters,
                                              jstring j_iceCandidates,
                                              jstring j_dtlsParameters) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);
    auto id = webrtc::JavaToStdString(env, j_id);
    auto s_iceParameters = webrtc::JavaToStdString(env, j_iceParameters);
    auto s_iceCandidates = webrtc::JavaToStdString(env, j_iceCandidates);
    auto s_dtlsParameters = webrtc::JavaToStdString(env, j_dtlsParameters);

    auto iceParameters = nlohmann::json::parse(s_iceParameters);
    auto iceCandidates = nlohmann::json::parse(s_iceCandidates);
    auto dtlsParameters = nlohmann::json::parse(s_dtlsParameters);

    auto listener = new TransportObserverJni();

    auto transport = device->CreateRecvTransport(
        listener,
        id,
        iceParameters,
        iceCandidates,
        dtlsParameters);

    return Java_RecvTransport_Constructor(env, webrtc::NativeToJavaPointer(transport));
}

