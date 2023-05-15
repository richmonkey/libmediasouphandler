#import "MSDevice.h"
#import "RTCConfiguration.h"
#import "RTCConfiguration+Private.h"
#import "RTCPeerConnectionFactory.h"
#import "RTCPeerConnectionFactory+Private.h"
#import "MSSendTransport.h"
#import "MSSendTransport+Private.h"

#import "MSRecvTransport.h"
#import "MSRecvTransport+Private.h"

#include "libmediasoupclient/include/Device.hpp"
#include "libmediasoupclient/include/Transport.hpp"

#include <json.hpp>


class TransportObserver: public mediasoupclient::Transport::Listener {
public:
    std::future<void> OnConnect(mediasoupclient::Transport* transport,
                                        const nlohmann::json& dtlsParameters) override {
        std::promise<void> promise;                                    
        promise.set_value();
        return promise.get_future();
    }
    
    void OnConnectionStateChange(mediasoupclient::Transport* transport,
                                         const std::string& connectionState) override {
        NSLog(@"OnConnectionStateChange:%s", connectionState.c_str());
    }
};

@implementation MSDevice {
    mediasoupclient::Device *_device;
}


-(instancetype)init {
    self = [super init];
    if (self) {

    }
    return self;
}

-(BOOL)loaded {
    return _device->IsLoaded();
}

-(NSString*)rtpCapabilities {
    return [NSString stringWithUTF8String:_device->GetRtpCapabilities().dump().c_str()];
}

-(NSString*)sctpCapabilities {
    return [NSString stringWithUTF8String:_device->GetSctpCapabilities().dump().c_str()];
}

-(BOOL)canProduce:(NSString*)kind {
    return _device->CanProduce([kind UTF8String]);
}

-(MSSendTransport*)createSendTransport:(NSString*)id 
             iceParameters:(NSString*)iceParameters 
             iceCandidates:(NSString*)iceCandidates 
            dtlsParameters:(NSString*)dtlsParameters 
                 rtcConfig:(RTC_OBJC_TYPE(RTCConfiguration) *)rtcConfig 
                   factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory {
    
    auto iceParametersJson = nlohmann::json::parse([iceParameters UTF8String]);
    auto iceCandidatesJson = nlohmann::json::parse([iceCandidates UTF8String]);
    auto dtlsParametersJson = nlohmann::json::parse([dtlsParameters UTF8String]);

   
    std::unique_ptr<webrtc::PeerConnectionInterface::RTCConfiguration> p_rtpConfig([rtcConfig createNativeConfiguration]);
    mediasoupclient::PeerConnection::Options options; 
    options.config = *p_rtpConfig;
    options.factory = factory.nativeFactory.get();

    auto listener = new TransportObserver();
    auto transport = _device->CreateSendTransport(listener, [id UTF8String], iceParametersJson, iceCandidatesJson, dtlsParametersJson, &options);

    return [[MSSendTransport alloc] initWithNativeTransport:transport factory:factory];
}

-(MSRecvTransport*)createRecvTransport:(NSString*)id 
             iceParameters:(NSString*)iceParameters 
             iceCandidates:(NSString*)iceCandidates 
            dtlsParameters:(NSString*)dtlsParameters 
                 rtcConfig:(RTC_OBJC_TYPE(RTCConfiguration) *)rtcConfig 
                   factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory {

    auto iceParametersJson = nlohmann::json::parse([iceParameters UTF8String]);
    auto iceCandidatesJson = nlohmann::json::parse([iceCandidates UTF8String]);
    auto dtlsParametersJson = nlohmann::json::parse([dtlsParameters UTF8String]);                    

    std::unique_ptr<webrtc::PeerConnectionInterface::RTCConfiguration> p_rtpConfig([rtcConfig createNativeConfiguration]);
    mediasoupclient::PeerConnection::Options options; 
    options.config = *p_rtpConfig;
    options.factory = factory.nativeFactory.get();

    auto listener = new TransportObserver();
    auto transport = _device->CreateRecvTransport(listener, [id UTF8String], iceParametersJson, iceCandidatesJson, dtlsParametersJson, &options);

    return [[MSRecvTransport alloc] initWithNativeTransport:transport factory:factory];
}


@end