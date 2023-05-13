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

// static jlong getNativeFactory(JNIEnv *env, jobject j_factory) {
//     jclass cls = env->GetObjectClass(j_factory);
//     jfieldID field = env->GetFieldID(cls, "nativeFactory", "J");
//     return env->GetLongField(j_factory, field);
// }

// static webrtc::PeerConnectionFactoryInterface* PeerConnectionFactoryFromJava(jlong j_p) {
//     return reinterpret_cast<webrtc::jni::OwnedFactoryAndThreads*>(j_p)->factory();
// }

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


JOWW(jlong, Device_nativeCreateDevice)(JNIEnv *env, jclass cls) {
    LOG("Device native create device");
    mediasoupclient::Device *device = new mediasoupclient::Device();
    return webrtc::NativeToJavaPointer(device);
}

JOWW(void, Device_nativeLoad)(JNIEnv *env,
                              jclass cls,
                              jlong nativeDevice,
                              jstring j_routerRtpCapabilities,
                              jobject rtcConfig,
                              jlong nativeFactory) {
    LOG("Device native load");
    auto s_routerRtpCapabilities = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(env, j_routerRtpCapabilities));
    auto routerRtpCapabilities = nlohmann::json::parse(s_routerRtpCapabilities);

    const webrtc::JavaParamRef<jobject> j_rtc_config(env, rtcConfig);
    webrtc::PeerConnectionInterface::RTCConfiguration rtc_config;
    webrtc::jni::JavaToNativeRTCConfiguration(env, j_rtc_config, &rtc_config);

    webrtc::PeerConnectionFactoryInterface *factory_interface = reinterpret_cast<webrtc::PeerConnectionFactoryInterface*>(nativeFactory);
    
    mediasoupclient::PeerConnection::Options options;
    options.config = rtc_config;
    options.factory = factory_interface;

    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);

    device->Load(routerRtpCapabilities, &options);
}


JOWW(jboolean, Device_nativeIsLoaded)(JNIEnv *env, jclass cls,
                                      jlong nativeDevice) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);    
    return device->IsLoaded() ? JNI_TRUE : JNI_FALSE;
}



JOWW(void, Device_nativeRelease)(JNIEnv *env, jclass cls, jlong nativeDevice) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);    
    delete device;
}


JOWW(jstring, Device_nativeGetRtpCapabilities)(JNIEnv *env, jclass cls, jlong nativeDevice) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);    
    auto rtpCap = device->GetRtpCapabilities();
    auto s_rtpCap = rtpCap.dump();
    auto result = webrtc::NativeToJavaString(env, s_rtpCap);
    return result.Release();
}


JOWW(jstring, Device_nativeGetSctpCapabilities)(JNIEnv *env, jclass cls, jlong nativeDevice) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);    
    auto sctpCap = device->GetSctpCapabilities();
    auto s_sctpCap = sctpCap.dump();
    auto result = webrtc::NativeToJavaString(env, s_sctpCap);
    return result.Release();
}


JOWW(jboolean, Device_nativeCanProduce)(JNIEnv *env, jclass cls,
                                       jlong nativeDevice, jstring kind) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);

    auto k = webrtc::JavaToStdString(env, kind);
    
    auto result = device->CanProduce(k);

    return result ? JNI_TRUE : JNI_FALSE;
}


JOWW(jobject, Device_nativeCreateSendTransport)(JNIEnv *env, 
                                              jclass cls,
                                              jlong nativeDevice,
                                              jstring j_id,
                                              jstring j_iceParameters,
                                              jstring j_iceCandidates,
                                              jstring j_dtlsParameters,
                                              jobject rtcConfig,
                                              jlong nativeFactory) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);
    auto id = webrtc::JavaToStdString(env, j_id);
    auto s_iceParameters = webrtc::JavaToStdString(env, j_iceParameters);
    auto s_iceCandidates = webrtc::JavaToStdString(env, j_iceCandidates);
    auto s_dtlsParameters = webrtc::JavaToStdString(env, j_dtlsParameters);

    auto iceParameters = nlohmann::json::parse(s_iceParameters);
    auto iceCandidates = nlohmann::json::parse(s_iceCandidates);
    auto dtlsParameters = nlohmann::json::parse(s_dtlsParameters);

    const webrtc::JavaParamRef<jobject> j_rtc_config(env, rtcConfig);
    webrtc::PeerConnectionInterface::RTCConfiguration rtc_config;
    webrtc::jni::JavaToNativeRTCConfiguration(env, j_rtc_config, &rtc_config);

    webrtc::PeerConnectionFactoryInterface *factory_interface = reinterpret_cast<webrtc::PeerConnectionFactoryInterface*>(nativeFactory);
    
    mediasoupclient::PeerConnection::Options options;
    options.config = rtc_config;
    options.factory = factory_interface;

    auto listener = new TransportObserverJni();

    auto transport = device->CreateSendTransport(
      listener,
      id,
      iceParameters,
      iceCandidates,
      dtlsParameters,
      &options);
    
    return Java_SendTransport_Constructor(env, webrtc::NativeToJavaPointer(transport));
}

JOWW(jobject, Device_nativeCreateRecvTransport)(JNIEnv *env,
                                              jclass cls,
                                              jlong nativeDevice,
                                              jstring j_id,
                                              jstring j_iceParameters,
                                              jstring j_iceCandidates,
                                              jstring j_dtlsParameters,
                                              jobject rtcConfig,
                                              jlong nativeFactory) {
    mediasoupclient::Device *device = reinterpret_cast<mediasoupclient::Device*>(nativeDevice);
    auto id = webrtc::JavaToStdString(env, j_id);
    auto s_iceParameters = webrtc::JavaToStdString(env, j_iceParameters);
    auto s_iceCandidates = webrtc::JavaToStdString(env, j_iceCandidates);
    auto s_dtlsParameters = webrtc::JavaToStdString(env, j_dtlsParameters);

    auto iceParameters = nlohmann::json::parse(s_iceParameters);
    auto iceCandidates = nlohmann::json::parse(s_iceCandidates);
    auto dtlsParameters = nlohmann::json::parse(s_dtlsParameters);

    const webrtc::JavaParamRef<jobject> j_rtc_config(env, rtcConfig);
    webrtc::PeerConnectionInterface::RTCConfiguration rtc_config;
    webrtc::jni::JavaToNativeRTCConfiguration(env, j_rtc_config, &rtc_config);

    webrtc::PeerConnectionFactoryInterface *factory_interface = reinterpret_cast<webrtc::PeerConnectionFactoryInterface*>(nativeFactory);
    
    mediasoupclient::PeerConnection::Options options;
    options.config = rtc_config;
    options.factory = factory_interface;

    auto listener = new TransportObserverJni();

    auto transport = device->CreateRecvTransport(
        listener,
        id,
        iceParameters,
        iceCandidates,
        dtlsParameters,
        &options);

    return Java_RecvTransport_Constructor(env, webrtc::NativeToJavaPointer(transport));
}

