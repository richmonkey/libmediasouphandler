#import "MSRecvTransport.h"
#import "MSRecvTransport+Private.h"
#import "MSTransport+Private.h"
#import "RTCMediaStreamTrack+Private.h"
#import "RTCRtpReceiver+Private.h"

@implementation MSRecvResult
-(instancetype)initWithLocalId:(NSString*)localId track:(RTC_OBJC_TYPE(RTCMediaStreamTrack) *)track rtpReceiver:(RTC_OBJC_TYPE(RTCRtpReceiver) *)rtpReceiver {
    self = [super init];
    if (self) {
        self.localId = localId;
        self.track = track;
        self.rtpReceiver = rtpReceiver;
    }
    return self;
}

@end

@implementation MSRecvTransport {
    
}


-(instancetype)initWithNativeTransport:(mediasoupclient::RecvTransport *)nativeTransport  factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory{
    self = [super initWithNativeTransport:nativeTransport factory:factory];
    if (self) {

    }
    return self;
}

-(mediasoupclient::RecvTransport*)recvTransport {
    return reinterpret_cast<mediasoupclient::RecvTransport*>(self.nativeTransport);
}

-(MSRecvResult*)consume:(NSString*)id 
           producerId:(NSString*)producerId 
                 kind:(NSString*)kind 
        rtpParameters:(NSString*)rtpParameters {
    auto rtpParametersJson = nlohmann::json::parse([rtpParameters UTF8String]);

    auto recvResult = self.recvTransport->Consume([id UTF8String], [producerId UTF8String], [kind UTF8String], &rtpParametersJson);

    RTC_OBJC_TYPE(RTCMediaStreamTrack) *track = [[RTC_OBJC_TYPE(RTCMediaStreamTrack) alloc] initWithFactory:self.factory nativeTrack:recvResult.track];
    RTC_OBJC_TYPE(RTCRtpReceiver) *rtpReceiver = [[RTC_OBJC_TYPE(RTCRtpReceiver) alloc] initWithFactory:self.factory nativeRtpReceiver:recvResult.rtpReceiver];
    NSString *localId = [NSString stringWithUTF8String:recvResult.localId.c_str()];

    MSRecvResult *result = [[MSRecvResult alloc] initWithLocalId:localId track:track rtpReceiver:rtpReceiver];

    return result;
}

@end