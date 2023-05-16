#import <Foundation/Foundation.h>
#import "MSTransport.h"
#import "RTCMediaStreamTrack.h"
#import "RTCRtpParameters.h"
#import "RTCRtpEncodingParameters.h"
#import "RTCRtpSender.h"

NS_ASSUME_NONNULL_BEGIN

RTC_OBJC_EXPORT 
@interface MSSendResult : NSObject
@property(nonatomic, copy) NSString *localId;
@property(nonatomic, copy) NSString *rtpParameters;
@property(nonatomic) RTC_OBJC_TYPE(RTCRtpSender) *rtpSender;
-(instancetype)initWithLocalId:(NSString*)localId rtpParameters:(NSString*)rtpParameters rtpSender:(RTC_OBJC_TYPE(RTCRtpSender) *)rtpSender;
@end

RTC_OBJC_EXPORT 
@interface MSSendTransport : MSTransport

-(MSSendResult*)produce:(RTC_OBJC_TYPE(RTCMediaStreamTrack) *)track 
     encodings:(NSArray<RTC_OBJC_TYPE(RTCRtpEncodingParameters)*>*)encodings
  codecOptions:(nullable NSString*)codecOptions 
         codec:(nullable NSString*)codec;

-(void)closeProducer:(NSString*)localId;
-(void)replaceTrack:(NSString*)localId track:(RTC_OBJC_TYPE(RTCMediaStreamTrack) *)track;
-(NSString*)getProducerStats:(NSString*)localId;
-(void)setMaxSpatialLayer:(NSString*)localId maxSpatialLayer:(int)maxSpatialLayer;
@end

NS_ASSUME_NONNULL_END