#import "MSTransport.h"
#import "RTCPeerConnectionFactory.h"
#include "libmediasoupclient/include/Transport.hpp"

@interface MSTransport()
-(instancetype)initWithNativeTransport:(mediasoupclient::Transport *)nativeTransport 
                               factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory;

@property(nonatomic, readonly) mediasoupclient::Transport *nativeTransport;
@property(nonatomic) RTC_OBJC_TYPE(RTCPeerConnectionFactory) *factory;

@end