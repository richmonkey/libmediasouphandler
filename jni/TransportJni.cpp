#include "libmediasoupclient/jni/TransportJni.hpp"
#include "sdk/android/native_api/jni/java_types.h"
#include "sdk/android/src/jni/pc/rtp_parameters.h"
#include "sdk/android/src/jni/pc/rtp_sender.h"
#include "sdk/android/src/jni/pc/rtp_receiver.h"
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
    jclass cls = env->FindClass("org/mediasoup/Recvransport$RecvResult");
    auto constructor = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;Lorg/webrtc/RtpReceiver;Lorg/webrtc/MediaStreamTrack;)V");
    auto object = env->NewObject(cls, constructor, localId, rtpReceiver, nativeTrack);
    return object;
}

// static jobject createMediaStreamTrack(JNIEnv* env, jlong nativeTrack) {
//     jclass cls = env->FindClass("org/mediasoup/Recvransport$RecvResult");
// }

//Transport
// JOWW(void, Transport_nativeClose)(JNIEnv *env, jlong nativeTransport) {
//     mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
//     transport->Close();
// }

// JOWW(jboolean, Transport_nativeIsClosed)(JNIEnv *env, jlong nativeTransport) {
//     mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
//     return transport->IsClosed();
// }

    
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
// JOWW(jlong, SendTransport_nativeProduce)(JNIEnv *env, jobject object,
//                                         jlong nativeTransport,
//                                         jobject j_producer,
//                                         jobject j_listener,
//                                         jlong nativeTrack,
//                                         jobjectArray j_encodings) {
//     TransportJni *jni_transport = reinterpret_cast<TransportJni*>(nativeTransport);
//     ProducerListenerJni *listener = new ProducerListenerJni(env, j_listener, j_producer);
//     webrtc::MediaStreamTrackInterface *track = reinterpret_cast<webrtc::MediaStreamTrackInterface*>(nativeTrack);

//     std::vector<webrtc::RtpEncodingParameters> encodings;
//     int len = env->GetArrayLength(j_encodings);
//     for (int i = 0; i < len; i++) {
//         jobject e = env->GetObjectArrayElement(j_encodings, i);
//         webrtc::ScopedJavaLocalRef<jobject> j_encoding = webrtc::ScopedJavaLocalRef<jobject>(env, e);
//         auto parameters = webrtc::jni::JavaToNativeRtpEncodingParameters(env, j_encoding);
//         encodings.push_back(parameters);
//     }
        
//     auto producer = jni_transport->send_transport->Produce(listener, track, &encodings, NULL);
    
//     return webrtc::NativeToJavaPointer(new ProducerJni(producer, listener));
// }

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
    auto s_codecOptions = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_codecOptions));
    auto s_codec = webrtc::JavaToNativeString(env, webrtc::JavaParamRef<jstring>(j_codec));
    auto codecOptions = nlohmann::json::parse(s_codecOptions);
    auto codec = nlohmann::json::parse(s_codec);

    auto sendResult = transport->Produce(track, &encodings, &codecOptions, &codec);
    auto localId = webrtc::NativeToJavaString(env, sendResult.localId);
    auto rtpParameters = webrtc::NativeToJavaString(env, sendResult.rtpParameters.dump());
    auto rtpSender = webrtc::jni::NativeToJavaRtpSender(env, sendResult.rtpSender);
    return Java_SendTransport_SendResult_Constructor(env, localId.Release(), rtpSender.Release(), rtpParameters.Release());
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



//Producer begin

// JOWW(jstring, Producer_nativeGetId)(JNIEnv *env, jobject object,
//                                  jlong nativeProducer) {

//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     auto id = producer->producer->GetId();
//     auto result = webrtc::NativeToJavaString(env, id);
//     return result.Release();
// }
                                 
// JOWW(jstring, Producer_nativeGetKind)(JNIEnv *env, jobject object,
//                                  jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     auto kind = producer->producer->GetKind();
//     auto result =  webrtc::NativeToJavaString(env, kind);
//     return result.Release();    
// }


// JOWW(void, Producer_nativeGetTrack)(JNIEnv *env, jobject object,
//                                  jlong nativeProducer) {
//     LOG("not implement");
// }


// JOWW(jstring, Producer_nativeGetRtpParameters)(JNIEnv *env, jobject object,
//                                                jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     auto rtpParameters = producer->producer->GetRtpParameters();
//     auto result =  webrtc::NativeToJavaString(env, rtpParameters.dump());
//     return result.Release();        
// }


// JOWW(jint, Producer_nativeGetMaxSpatialLayer)(JNIEnv *env, jobject object,
//                                             jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     auto maxSpatialLayer = producer->producer->GetMaxSpatialLayer();
//     return maxSpatialLayer;
// }


// JOWW(jstring, Producer_nativeGetStats)(JNIEnv *env, jobject object,
//                                             jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     auto stats = producer->producer->GetStats();
//     auto result =  webrtc::NativeToJavaString(env, stats.dump());
//     return result.Release();      
// }

// JOWW(jstring, Producer_nativeGetAppData)(JNIEnv *env, jobject object,
//                                       jlong nativeProducer) {
//     return NULL;
// }


// JOWW(jboolean, Producer_nativeIsClosed)(JNIEnv *env, jobject object,
//                                     jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     auto closed = producer->producer->IsClosed();
//     return closed ? JNI_TRUE : JNI_FALSE;
// }


// JOWW(jboolean, Producer_nativeIsPaused)(JNIEnv *env, jobject object,
//                                     jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     auto paused = producer->producer->IsPaused();
//     return paused ? JNI_TRUE : JNI_FALSE;    
// }



// JOWW(void, Producer_nativeClose)(JNIEnv *env, jobject object,
//                                     jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     producer->producer->Close();
//     delete producer;
// }


// JOWW(void, Producer_nativePause)(JNIEnv *env, jobject object,
//                                     jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     producer->producer->Pause();        
// }


// JOWW(void, Producer_nativeResume)(JNIEnv *env, jobject object,
//                                     jlong nativeProducer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     producer->producer->Resume();            
// }


// JOWW(void, Producer_nativeReplaceTrack)(JNIEnv *env, jobject object,
//                                         jlong nativeProducer,
//                                         jlong nativeTrack) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     LOG("not implement");
// }


// JOWW(void, Producer_nativeSetMaxSpatialLayer)(JNIEnv *env, jobject object,
//                                               jlong nativeProducer,
//                                               jint spatialLayer) {
//     ProducerJni *producer = reinterpret_cast<ProducerJni*>(nativeProducer);
//     producer->producer->SetMaxSpatialLayer(spatialLayer);
// }


//Consumer

// JOWW(jstring, Consumer_nativeGetId)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {
//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     auto id = consumer->consumer->GetId();
//     auto result = webrtc::NativeToJavaString(env, id);
//     return result.Release();    
// }


// JOWW(jstring, Consumer_nativeGetProducerId)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {

//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     auto producerId = consumer->consumer->GetProducerId();
//     auto result = webrtc::NativeToJavaString(env, producerId);
//     return result.Release();    
// }


// JOWW(jstring, Consumer_nativeGetKind)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {

//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     auto kind = consumer->consumer->GetKind();
//     auto result = webrtc::NativeToJavaString(env, kind);
//     return result.Release();        
// }

// JOWW(jstring, Consumer_nativeGetRtpParameters)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {

//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);    
//     auto rtpParameters = consumer->consumer->GetRtpParameters();
//     auto result =  webrtc::NativeToJavaString(env, rtpParameters.dump());
//     return result.Release();            
// }


// JOWW(void, Consumer_nativeGetTrack)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {

//     LOG("not implement");
// }



// JOWW(jstring, Consumer_nativeGetStats)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {
//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     auto stats = consumer->consumer->GetStats();
//     auto result = webrtc::NativeToJavaString(env, stats.dump());
//     return result.Release();          
// }



// JOWW(jstring, Consumer_nativeGetAppData)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {

//     return nullptr;
// }


// JOWW(jboolean, Consumer_nativeIsClosed)(JNIEnv *env, jobject object,
//                                        jlong nativeConsumer) {
//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     bool closed = consumer->consumer->IsClosed();
//     return closed ? JNI_TRUE : JNI_FALSE;
// }

// JOWW(jboolean, Consumer_nativeIsPaused)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {
//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     bool paused = consumer->consumer->IsPaused();
//     return paused ? JNI_TRUE : JNI_FALSE;    
// }


// JOWW(void, Consumer_nativeClose)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {

//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     consumer->consumer->Close();
//     delete consumer;
// }


// JOWW(void, Consumer_nativePause)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {
//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     consumer->consumer->Pause();
// }

// JOWW(void, Consumer_nativeResume)(JNIEnv *env, jobject object,
//                                     jlong nativeConsumer) {
//     ConsumerJni *consumer = reinterpret_cast<ConsumerJni*>(nativeConsumer);
//     consumer->consumer->Resume();    
// }


// //called in other thread
// JOWW(void, Callback_nativeCall)(JNIEnv *env, jobject object,
//                                    jlong nativeCallback, jstring value) {

//     if (value != NULL) {
//         CallbackStdString *cb = reinterpret_cast<CallbackStdString*>(nativeCallback);
//         auto v = webrtc::JavaToStdString(env, value);
//         cb->Call(v);
//         delete cb;
//     } else {
//         CallbackVoid *cb = reinterpret_cast<CallbackVoid*>(nativeCallback);
//         cb->Call();
//         delete cb;        
//     }
// }


// //called in other thread
// JOWW(void, Callback_nativeCallException)(JNIEnv *env, jobject object,
//                                    jlong nativeCallback, jstring value) {
//     CallbackInterface *cb = reinterpret_cast<CallbackInterface*>(nativeCallback);    
//     auto v = webrtc::JavaToStdString(env, value);
//     cb->CallException(v);
//     delete cb;
// }
