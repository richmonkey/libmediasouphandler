#import <Foundation/Foundation.h>
#import "MSTransport.h"
#import "RTCMediaStreamTrack.h"
#import "RTCRtpReceiver.h"

RTC_OBJC_EXPORT 
@interface MSRecvResult : NSObject
@property(nonatomic, copy) NSString *localId;
@property(nonatomic) RTC_OBJC_TYPE(RTCMediaStreamTrack) *track;
@property(nonatomic) RTC_OBJC_TYPE(RTCRtpReceiver) *rtpReceiver;

-(instancetype)initWithLocalId:(NSString*)localId track:(RTC_OBJC_TYPE(RTCMediaStreamTrack) *)track rtpReceiver:(RTC_OBJC_TYPE(RTCRtpReceiver) *)rtpReceiver;

@end

RTC_OBJC_EXPORT 
@interface MSRecvTransport : MSTransport

-(MSRecvResult*)consume:(NSString*)id 
           producerId:(NSString*)producerId 
                 kind:(NSString*)kind 
        rtpParameters:(NSString*)rtpParameters;

-(void)closeConsumer:(NSString*)localId;
-(NSString*)GetConsumerStats:(NSString*)localId;
@end
