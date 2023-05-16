#import <Foundation/Foundation.h>
#import "RTCMacros.h"

@class MSSendTransport;
@class MSRecvTransport;

@class RTC_OBJC_TYPE(RTCConfiguration);
@class RTC_OBJC_TYPE(RTCPeerConnectionFactory);

RTC_OBJC_EXPORT
@interface MSDevice : NSObject

@property(nonatomic, readonly) BOOL loaded;
@property(nonatomic, readonly) NSString *rtpCapabilities;
@property(nonatomic, readonly) NSString *sctpCapabilities;

-(instancetype)init;

-(void)load:(NSString*)routerRtpCapabilities rtcConfig:(RTC_OBJC_TYPE(RTCConfiguration) *)rtcConfig 
        factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory;

-(BOOL)canProduce:(NSString*)kind;

-(MSSendTransport*)createSendTransport:(NSString*)id 
             iceParameters:(NSString*)iceParameters 
             iceCandidates:(NSString*)iceCandidates 
            dtlsParameters:(NSString*)dtlsParameters 
                 rtcConfig:(RTC_OBJC_TYPE(RTCConfiguration) *)rtcConfig 
                   factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory;

-(MSRecvTransport*)createRecvTransport:(NSString*)id 
             iceParameters:(NSString*)iceParameters 
             iceCandidates:(NSString*)iceCandidates 
            dtlsParameters:(NSString*)dtlsParameters 
                 rtcConfig:(RTC_OBJC_TYPE(RTCConfiguration) *)rtcConfig 
                   factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory;

@end