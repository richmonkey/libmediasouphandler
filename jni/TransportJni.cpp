#include "libmediasoupclient/jni/TransportJni.hpp"
#include "sdk/android/native_api/jni/java_types.h"
#include "sdk/android/src/jni/pc/rtp_parameters.h"
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


//Transport
// JOWW(void, Transport_nativeClose)(JNIEnv *env, jlong nativeTransport) {
//     mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
//     transport->Close();
// }

// JOWW(jboolean, Transport_nativeIsClosed)(JNIEnv *env, jlong nativeTransport) {
//     mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
//     return transport->IsClosed();
// }

    
JOWW(jstring, Transport_nativeGetId)(JNIEnv *env, jobject object,
                                     jlong nativeTransport) {
    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    std::string id = transport->GetId();
    auto result = webrtc::NativeToJavaString(env, id);
    return result.Release();
}

JOWW(jstring, Transport_nativeGetConnectionState)(JNIEnv *env, jobject object,
                                                  jlong nativeTransport) {

    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    auto state = transport->GetConnectionState();
    auto result = webrtc::NativeToJavaString(env, state);

    return result.Release();    
}

JOWW(jstring, Transport_nativeGetStats)(JNIEnv *env, jobject object,
                                     jlong nativeTransport) {
    mediasoupclient::Transport *transport = reinterpret_cast<mediasoupclient::Transport*>(nativeTransport);
    auto stats = transport->GetStats();
    auto result = webrtc::NativeToJavaString(env, stats.dump());

    return result.Release();
}    

    
JOWW(jboolean, Transport_nativeIsClosed)(JNIEnv *env, jobject object,
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

JOWW(void, Transport_nativeRestartIce)(JNIEnv *env, jobject object,
                                       jlong nativeTransport,
                                       jstring iceParameters) {
}     


JOWW(void, Transport_nativeUpdateIceServers)(JNIEnv *env, jobject object,
                                            jlong nativeTransport,
                                            jstring iceServers) {
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

JOWW(void, SendTransport_nativeProduce)(
    JNIEnv *env, 
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
    auto s_codecOptions = webrtc::JavaToStdString(env, j_codecOptions);
    auto s_codec = webrtc::JavaToStdString(env, j_codec);
    auto codecOptions = nlohmann::json::parse(s_codecOptions);
    auto codec = nlohmann::json::parse(s_codec);


    transport->Produce(track, &encodings, &codecOptions, &codec);
}


    
    
//RecvTransport
// JOWW(jlong, RecvTransport_nativeConsume)(JNIEnv *env, jobject object,
//                                         jlong nativeTransport,
//                                         jobject j_consumer,
//                                         jobject j_listener,
//                                         jstring j_id,
//                                         jstring j_producerId,
//                                         jstring j_kind,
//                                         jstring j_rtpParameters,
//                                         jstring j_appData) {
//     TransportJni *jni_transport = reinterpret_cast<TransportJni*>(nativeTransport);

//     ConsumerListenerJni *listener = new ConsumerListenerJni(env, j_listener, j_consumer);
//     auto id = webrtc::JavaToStdString(env, j_id);
//     auto producerId = webrtc::JavaToStdString(env, j_producerId);
//     auto kind = webrtc::JavaToStdString(env, j_kind);
//     auto s_rtpParameters = webrtc::JavaToStdString(env, j_rtpParameters);
//     auto rtpParameters = nlohmann::json::parse(s_rtpParameters);
//     auto consumer =  jni_transport->recv_transport->Consume(listener, id, producerId, kind, &rtpParameters);

//     return webrtc::NativeToJavaPointer(new ConsumerJni(consumer, listener));
// }



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
