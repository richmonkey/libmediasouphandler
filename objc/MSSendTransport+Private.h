#import "MSSendTransport.h"
#import "RTCPeerConnectionFactory.h"
#include "libmediasoupclient/include/Transport.hpp"

@interface MSSendTransport()
-(instancetype)initWithNativeTransport:(mediasoupclient::SendTransport *)nativeTransport factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory;

@property(nonatomic, readonly) mediasoupclient::SendTransport *sendTransport;
@end