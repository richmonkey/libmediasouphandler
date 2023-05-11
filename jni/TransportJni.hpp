#ifndef MSC_TRANSPORT_JNI_HPP
#define MSC_TRANSPORT_JNI_HPP
#include <jni.h>
#include <memory>
// #include "sdk/android/native_api/jni/scoped_java_ref.h"
// #include "sdk/android/native_api/jni/java_types.h"
#include "libmediasoupclient/include/Transport.hpp"
// #include "libmediasoupclient/jni/Callback.hpp"

// class TransportObserverJni;

/*
class TransportJni : public Transport::Listener {
public:
    #if 0
    static jclass sendtransport_class;
    static jmethodID sendtransport_constructor_method;

    static jclass recvtransport_class;
    static jmethodID recvtransport_constructor_method;

    static void Load(JNIEnv *env) {
        jclass cls = env->FindClass("org/mediasoup/SendTransport");
        webrtc::ScopedJavaGlobalRef<jclass> s_clazz(webrtc::ScopedJavaLocalRef<jclass>(env, cls));
        sendtransport_class = s_clazz.Release();
        sendtransport_constructor_method = env->GetMethodID(sendtransport_class, "<init>", "(J)V");

        
        cls = env->FindClass("org/mediasoup/RecvTransport");
        webrtc::ScopedJavaGlobalRef<jclass> r_clazz(webrtc::ScopedJavaLocalRef<jclass>(env, cls));
        recvtransport_class = r_clazz.Release();
        recvtransport_constructor_method = env->GetMethodID(recvtransport_class, "<init>", "(J)V");
    }
    #endif

  
    
public:
    TransportJni(mediasoupclient::SendTransport *t, TransportObserverJni *l)
        : listener(l), send_transport(t),
          recv_transport(NULL), transport(t) {
    }

    TransportJni(mediasoupclient::RecvTransport *t, TransportObserverJni *l)
        : listener(l), send_transport(NULL),
          recv_transport(t), transport(t) {
    }

 
    ~TransportJni() {
        delete send_transport;
        delete recv_transport;
    }
    
    std::unique_ptr<TransportObserverJni> listener;
    mediasoupclient::SendTransport *send_transport;
    mediasoupclient::RecvTransport *recv_transport;
    mediasoupclient::Transport *transport;
};
*/    

class TransportObserverJni: public mediasoupclient::Transport::Listener {
public:
    std::future<void> OnConnect(mediasoupclient::Transport* transport,
                                        const nlohmann::json& dtlsParameters) override {
        std::promise<void> promise;                                    
        promise.set_value();
        return promise.get_future();
    }
    
    void OnConnectionStateChange(mediasoupclient::Transport* transport,
                                         const std::string& connectionState) override {
    }
};

/*
class TransportObserverJni {
public:
    static jclass transport_listener_class;
    static jmethodID onconnect_method;
    static jmethodID onconnectionstatechange_method;

    static void Load(JNIEnv *env) {
        jclass cls = env->FindClass("org/mediasoup/Transport$Listener");
        webrtc::ScopedJavaGlobalRef<jclass> clazz(webrtc::ScopedJavaLocalRef<jclass>(env, cls));
        transport_listener_class = clazz.Release();

        onconnect_method = env->GetMethodID(transport_listener_class,
                                            "onConnect",
                                            "(Lorg/mediasoup/Transport;Ljava/lang/String;Lorg/mediasoup/Callback)V");
        onconnectionstatechange_method = env->GetMethodID(transport_listener_class,
                                                          "OnConnectionStateChange",
                                                          "(Lorg/mediasoup/Transport;Ljava/lang/String;)V");
    }
    
public:
	TransportObserverJni(JNIEnv *jni, jobject j_observer, jobject j_transport)
		: j_observer_(jni, webrtc::JavaParamRef<jobject>(j_observer)),
          j_transport_(jni, webrtc::JavaParamRef<jobject>(j_transport)) {
		
	}

    virtual ~TransportObserverJni() {}
 
  
    virtual std::future<void> OnConnect(mediasoupclient::Transport* transport,
                                        const nlohmann::json& dtlsParameters) {
        JNIEnv* env = webrtc::AttachCurrentThreadIfNeeded();
        auto j_params = webrtc::NativeToJavaString(env, dtlsParameters.dump());
        
        CallbackVoid *cb = new CallbackVoid();
        jobject j_cb = CallbackJni::CreateCallback(env, cb);
        
        env->CallVoidMethod(j_observer_.obj(),
                            onconnect_method,
                            j_transport_.obj(),
                            j_params.obj(),
                            j_cb);
        return cb->get_future();
    }
    
    virtual void OnConnectionStateChange(mediasoupclient::Transport* transport,
                                         const std::string& connectionState) {
        JNIEnv* env = webrtc::AttachCurrentThreadIfNeeded();

        auto state = webrtc::NativeToJavaString(env, connectionState);

        env->CallVoidMethod(j_observer_.obj(), onconnectionstatechange_method,
                            j_transport_.obj(), state.obj());
    }


protected:
	const webrtc::ScopedJavaGlobalRef<jobject> j_observer_;
	const webrtc::ScopedJavaGlobalRef<jobject> j_transport_;	    
};
    
class SendTransportObserverJni : public TransportObserverJni,
                                 public virtual mediasoupclient::SendTransport::Listener {

public:
    static jclass sendtransport_listener_class;
    static jmethodID onproduce_method;
    static jmethodID onproducedata_method;
    

    static void Load(JNIEnv *env) {
        jclass cls = env->FindClass("org/mediasoup/SendTransport$Listener");
        webrtc::ScopedJavaGlobalRef<jclass> clazz(webrtc::ScopedJavaLocalRef<jclass>(env, cls));
        transport_listener_class = clazz.Release();

        onproduce_method = env->GetMethodID(transport_listener_class,
                                            "onProduce",
                                            "(Lorg/mediasoup/Transport;Ljava/lang/String;Ljava/lang/String;Lorg/mediasoup/Callback)V");
        onconnectionstatechange_method = env->GetMethodID(transport_listener_class,
                                                          "OnConnectionStateChange",
                                                          "(Lorg/mediasoup/SendTransport;Ljava/lang/String;)V");        
    }
public:
    
    SendTransportObserverJni(JNIEnv *jni, jobject j_observer, jobject j_transport)
        : TransportObserverJni(jni, j_observer, j_transport) {
        
    }
    
    virtual std::future<void> OnConnect(mediasoupclient::Transport* transport,
                                        const nlohmann::json& dtlsParameters) {
      return TransportObserverJni::OnConnect(transport, dtlsParameters);
  
    }
    
    virtual void OnConnectionStateChange(mediasoupclient::Transport* transport,
                                         const std::string& connectionState) {
        return TransportObserverJni::OnConnectionStateChange(transport, connectionState);
    }

	virtual std::future<std::string> OnProduce(
              mediasoupclient::SendTransport* transport,
			  const std::string& kind,
			  nlohmann::json rtpParameters,
			  const nlohmann::json& appData) {

        JNIEnv* env = webrtc::AttachCurrentThreadIfNeeded();
        auto j_kind = webrtc::NativeToJavaString(env, kind);

        auto j_rtpParameters = webrtc::NativeToJavaString(env, rtpParameters.dump());
  
        CallbackStdString *cb = new CallbackStdString();
        jobject j_cb = CallbackJni::CreateCallback(env, cb);
        
        env->CallVoidMethod(j_observer_.obj(),
                            onproduce_method,
                            j_transport_.obj(),
                            j_kind.obj(),
                            j_rtpParameters.obj(),
                            j_cb);
        
        return cb->get_future();        
    }

    virtual std::future<std::string> OnProduceData(
              mediasoupclient::SendTransport* transport,
			  const nlohmann::json& sctpStreamParameters,
			  const std::string& label,
			  const std::string& protocol,
			  const nlohmann::json& appData) {
        std::promise<std::string> promise;

        try {
            throw std::runtime_error("not implement");
        } catch (std::exception& e) {
            promise.set_exception(std::current_exception());            
        }
        return promise.get_future();
    }
};

class RecvTransportObserverJni : public TransportObserverJni,
                                 public mediasoupclient::RecvTransport::Listener {

public:
    
    RecvTransportObserverJni(JNIEnv *jni, jobject j_observer, jobject j_transport)
        : TransportObserverJni(jni, j_observer, j_transport) {
        
    }
    
    virtual std::future<void> OnConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters) {
      return TransportObserverJni::OnConnect(transport, dtlsParameters);
  
    }
    
    virtual void OnConnectionStateChange(mediasoupclient::Transport* transport, const std::string& connectionState) {
        return TransportObserverJni::OnConnectionStateChange(transport, connectionState);
    }
};
*/

#endif
