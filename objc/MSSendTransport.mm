#import "MSSendTransport.h"
#import "MSSendTransport+Private.h"
#import "MSTransport+Private.h"
#import "RTCRtpEncodingParameters+Private.h"
#import "RTCMediaStreamTrack+Private.h"
#import "RTCRtpSender+Private.h"

@implementation MSSendResult 
-(instancetype)initWithLocalId:(NSString*)localId rtpParameters:(NSString*)rtpParameters rtpSender:(RTC_OBJC_TYPE(RTCRtpSender) *)rtpSender {
    self = [super init];
    if (self) {
        self.localId = localId;
        self.rtpParameters = rtpParameters;
        self.rtpSender = rtpSender;
    }
    return self;
}
@end

@implementation MSSendTransport {

}

-(instancetype)initWithNativeTransport:(mediasoupclient::SendTransport *)nativeTransport factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory {
    self = [super initWithNativeTransport:nativeTransport factory:factory];
    if (self) {

    }
    return self;
}

-(mediasoupclient::SendTransport*)sendTransport {
    return reinterpret_cast<mediasoupclient::SendTransport*>(self.nativeTransport);
}

-(MSSendResult*)produce:(RTC_OBJC_TYPE(RTCMediaStreamTrack) *)track 
     encodings:(NSArray<RTC_OBJC_TYPE(RTCRtpEncodingParameters) *> *)encodings 
  codecOptions:(nullable NSString*)codecOptions 
         codec:(nullable NSString*)codec {
    std::vector<webrtc::RtpEncodingParameters> nativeEncodings;
    for (NSUInteger i = 0; i < encodings.count; i++) {
        RTC_OBJC_TYPE(RTCRtpEncodingParameters) *encoding = encodings[i];
        nativeEncodings.push_back(encoding.nativeParameters);
    }

    nlohmann::json codecOptionsJson;
    if (!codecOptions) {
        codecOptionsJson = nlohmann::json::parse([codecOptions UTF8String]);
    }

    nlohmann::json codecJson;
    if (!codec) {
        codecJson = nlohmann::json::parse([codec UTF8String]);
    }

    auto produceResult = self.sendTransport->Produce(track.nativeTrack.get(), &nativeEncodings,
        codecOptionsJson.empty() ? nullptr : &codecOptionsJson, codecJson.empty() ? nullptr : &codecJson);

    NSString *localId = [NSString stringWithUTF8String:produceResult.localId.c_str()];
    RTC_OBJC_TYPE(RTCRtpSender) *rtpSender = [[RTC_OBJC_TYPE(RTCRtpSender) alloc] initWithFactory:self.factory nativeRtpSender:produceResult.rtpSender];
    NSString *rtpParameters = [NSString stringWithUTF8String:produceResult.rtpParameters.dump().c_str()];
    MSSendResult *result = [[MSSendResult alloc] initWithLocalId:localId rtpParameters:rtpParameters rtpSender:rtpSender];
    return result;
}

@end