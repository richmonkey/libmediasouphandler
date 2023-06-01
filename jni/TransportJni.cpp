#include <android/log.h>
#include "sdk/android/native_api/jni/java_types.h"
#include "sdk/android/src/jni/pc/rtp_parameters.h"
#include "sdk/android/src/jni/pc/rtp_sender.h"
#include "sdk/android/src/jni/pc/rtp_receiver.h"
#include "libmediasoupclient/include/Transport.hpp"
#include "libmediasoupclient/include/Device.hpp"
#include "libmediasoupclient/include/Transport.hpp"
#include "libmediasoupclient/jni/jni_helper.hpp"
#include <json.hpp>


static jlong getNativeTrack(JNIEnv *env, jobject j_track) {
    jclass cls = env->GetObjectClass(j_track);
    jmethodID method = env->GetMethodID(cls, "getNativeMediaStreamTrack", "()J");
    jlong nativeTrack = env->CallLongMethod(j_track, method);
    return nativeTrack;
}

static jobject Java_Fingerprint_Constructor(JNIEnv *env, jstring algo, jstring fingerprint) {
    jclass cls = env->FindClass("org/mediasoup/Fingerprint");
    auto constructor = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    auto object = env->NewObject(cls, constructor, algo, fingerprint);
    return object;
}

static jobject Java_SendTransport_SendResult_Constructor(JNIEnv* env, jstring localId, jobject rtpSender, jstring rtpParameters) {
    jclass cls = env->FindClass("org/mediasoup/SendTransport$SendResult");
    auto constructor = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;Lorg/webrtc/RtpSender;Ljava/lang/String;)V");
    auto object = env->NewObject(cls, constructor, localId, rtpSender, rtpParameters);
    return object;
}

static jobject Java_RecvTransport_RecvResult_Constructor(JNIEnv* env, jstring localId, jobject rtpReceiver, jlong nativeTrack) {
    jclass cls = env->FindClass("org/mediasoup/RecvTransport$RecvResult");
    auto constructor = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;Lorg/webrtc/RtpReceiver;J)V");
    auto object = env->NewObject(cls, constructor, localId, rtpReceiver, nativeTrack);
    return object;
}

    
JOWW(jstring, Transport_nativeGetId)(JNIEnv *env, jclass cls,
                                     jlong nativeTransport) {
    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    std::string id = transport->GetId();
    auto result = webrtc::NativeToJavaString(env, id);
    return result.Release();
}

JOWW(jstring, Transport_nativeGetConnectionState)(JNIEnv *env, jclass cls,
                                                  jlong nativeTransport) {

    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    auto state = transport->GetConnectionState();
    auto result = webrtc::NativeToJavaString(env, state);

    return result.Release();    
}

JOWW(jstring, Transport_nativeGetStats)(JNIEnv *env, jclass cls,
                                     jlong nativeTransport) {
    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    auto stats = transport->GetStats();
    auto result = webrtc::NativeToJavaString(env, stats.dump());

    return result.Release();
}    

JOWW(jobject, Transport_nativeGetFingerprint)(JNIEnv *env, jclass cls,
                                     jlong nativeTransport) {
    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    std::string alg, fingerprint;

    transport->GetFingerprint(alg, fingerprint);

    auto j_alg = webrtc::NativeToJavaString(env, alg);
    auto j_fingerprint = webrtc::NativeToJavaString(env, fingerprint);
    return Java_Fingerprint_Constructor(env, j_alg.Release(), j_fingerprint.Release());
}
    
JOWW(jboolean, Transport_nativeIsClosed)(JNIEnv *env, jclass cls,
                                        jlong nativeTransport) {

    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    auto closed = transport->IsClosed();
    return closed ? JNI_TRUE : JNI_FALSE;
} 

JOWW(void, Transport_nativeClose)(JNIEnv *env, jobject object,
                                  jlong nativeTransport) {
    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    transport->Close();
    delete transport->GetListener();
    delete transport;
}

JOWW(void, Transport_nativeRestartIce)(JNIEnv *env, jclass cls,
                                       jlong nativeTransport,
                                       jstring j_iceParameters) {
    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    auto s_iceParameters = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_iceParameters));
    auto iceParameters = nlohmann::json::parse(s_iceParameters);
    transport->RestartIce(iceParameters);
}     


JOWW(void, Transport_nativeUpdateIceServers)(JNIEnv *env, jclass cls,
                                            jlong nativeTransport,
                                            jstring j_iceServers) {
    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    auto s_iceServers = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_iceServers));
    auto iceServers = nlohmann::json::parse(s_iceServers);
    transport->UpdateIceServers(iceServers);                                                
}


//SendTransport
JOWW(jobject, SendTransport_nativeProduce)(
    JNIEnv *env, 
    jclass cls,
    jlong nativeTransport, 
    jobject j_track, 
    jobject j_encodings, 
    jstring j_codecOptions, 
    jstring j_codec) {
    mediasoupclient::SendTransport *transport = reinterpret_cast<mediasoupclient::SendTransport*>(nativeTransport);
    jlong nativeTrack = getNativeTrack(env, j_track);
    webrtc::MediaStreamTrackInterface *track = reinterpret_cast<webrtc::MediaStreamTrackInterface*>(nativeTrack);
    auto encodings = webrtc::JavaListToNativeVector<webrtc::RtpEncodingParameters, jobject>(env, webrtc::JavaParamRef<jobject>(j_encodings), 
        &webrtc::jni::JavaToNativeRtpEncodingParameters);
    nlohmann::json codecOptions;
    if (j_codecOptions != nullptr) {
        auto s_codecOptions = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_codecOptions));
        codecOptions = nlohmann::json::parse(s_codecOptions);
    }
    nlohmann::json codec;
    if (j_codec != nullptr) {
        auto s_codec = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_codec));
        codec = nlohmann::json::parse(s_codec);
    }

    auto sendResult = transport->Produce(track, &encodings, 
        codecOptions.empty() ? nullptr : &codecOptions, codec.empty() ? nullptr : &codec);

    auto localId = webrtc::NativeToJavaString(env, sendResult.localId);
    auto rtpParameters = webrtc::NativeToJavaString(env, sendResult.rtpParameters.dump());
    auto rtpSender = webrtc::jni::NativeToJavaRtpSender(env, sendResult.rtpSender);
    return Java_SendTransport_SendResult_Constructor(env, localId.Release(), rtpSender.Release(), rtpParameters.Release());
}


JOWW(void, SendTransport_nativeCloseProducer)(JNIEnv *env, jclass cls, jlong nativeTransport, jstring j_localId) {
    mediasoupclient::SendTransport *transport = reinterpret_cast<mediasoupclient::SendTransport*>(nativeTransport);
    auto localId = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_localId));
    transport->CloseProducer(localId);
}

JOWW(void, SendTransport_nativeReplaceTrack)(
    JNIEnv *env, 
    jclass cls, 
    jlong nativeTransport, 
    jstring j_localId, 
    jobject j_track) {
    mediasoupclient::SendTransport *transport = reinterpret_cast<mediasoupclient::SendTransport*>(nativeTransport);
    auto localId = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_localId));        
    jlong nativeTrack = getNativeTrack(env, j_track);
    webrtc::MediaStreamTrackInterface *track = reinterpret_cast<webrtc::MediaStreamTrackInterface*>(nativeTrack);    
    transport->ReplaceTrack(localId, track);
}

JOWW(void, SendTransport_nativeSetMaxSpatialLayer)(
    JNIEnv *env, 
    jclass cls, 
    jlong nativeTransport, 
    jstring j_localId, 
    jint maxSpatialLayer) {
    mediasoupclient::SendTransport *transport = reinterpret_cast<mediasoupclient::SendTransport*>(nativeTransport);
    auto localId = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_localId));        
    transport->SetMaxSpatialLayer(localId, maxSpatialLayer);
}

JOWW(jstring, SendTransport_nativeGetProducerStats)(
    JNIEnv *env, 
    jclass cls, 
    jlong nativeTransport, 
    jstring j_localId, 
    jint maxSpatialLayer) {
    mediasoupclient::SendTransport *transport = reinterpret_cast<mediasoupclient::SendTransport*>(nativeTransport);
    auto localId = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_localId));        
    auto stats = transport->GetProducerStats(localId);
    auto result = webrtc::NativeToJavaString(env, stats.dump());
    return result.Release();    
}
    
//RecvTransport
JOWW(jobject, RecvTransport_nativeConsume)(JNIEnv *env,
                                        jclass cls,
                                        jlong nativeTransport,
                                        jstring j_id,
                                        jstring j_producerId,
                                        jstring j_kind,
                                        jstring j_rtpParameters) {
    mediasoupclient::RecvTransport* transport = reinterpret_cast<mediasoupclient::RecvTransport*>(nativeTransport);


    auto id = webrtc::JavaToStdString(env, j_id);
    auto producerId = webrtc::JavaToStdString(env, j_producerId);
    auto kind = webrtc::JavaToStdString(env, j_kind);
    auto s_rtpParameters = webrtc::JavaToStdString(env, j_rtpParameters);
    auto rtpParameters = nlohmann::json::parse(s_rtpParameters);
    auto recvResult = transport->Consume(id, producerId, kind, &rtpParameters);

    auto localId = webrtc::NativeToJavaString(env, recvResult.localId);
    auto rtpReceiver = webrtc::jni::NativeToJavaRtpReceiver(env, recvResult.rtpReceiver);
    auto track = recvResult.track;
    jlong nativeTrack = webrtc::NativeToJavaPointer(track.release());

    return Java_RecvTransport_RecvResult_Constructor(env, localId.Release(), rtpReceiver.Release(), nativeTrack);
}

JOWW(void, RecvTransport_nativeCloseConsumer)(
    JNIEnv *env,
    jclass cls,
    jlong nativeTransport,
    jstring j_localId) {
    mediasoupclient::RecvTransport* transport = reinterpret_cast<mediasoupclient::RecvTransport*>(nativeTransport);
    auto localId = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_localId));    

    transport->CloseConsumer(localId);
}

JOWW(jstring, RecvTransport_nativeGetConsumerStats)(
    JNIEnv *env,
    jclass cls,
    jlong nativeTransport,
    jstring j_localId) {
    mediasoupclient::RecvTransport* transport = reinterpret_cast<mediasoupclient::RecvTransport*>(nativeTransport);
    auto localId = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_localId));    

    auto stats = transport->GetConsumerStats(localId);
    auto result = webrtc::NativeToJavaString(env, stats.dump());
    return result.Release();   
}

