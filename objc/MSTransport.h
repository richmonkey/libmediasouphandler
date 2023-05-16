#import <Foundation/Foundation.h>
#import "RTCMacros.h"

RTC_OBJC_EXPORT 
@interface MSFingerprint : NSObject
@property(nonatomic, copy) NSString *algorithm;
@property(nonatomic, copy) NSString *fingerprint;
-(instancetype)initWithAlgorithm:(NSString*)algorithm fingerprint:(NSString*)fingerprint;
@end

RTC_OBJC_EXPORT 
@interface MSTransport : NSObject

@property(nonatomic, readonly) MSFingerprint *fingerprint;

@end