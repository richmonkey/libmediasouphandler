#import <Foundation/Foundation.h>
#import "RTCMacros.h"
RTC_OBJC_EXPORT 
@interface MSClient : NSObject

+(void)initialize;
+(void)cleanup;
+(NSString*)version;

@end