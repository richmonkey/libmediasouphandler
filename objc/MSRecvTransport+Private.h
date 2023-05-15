#import "MSRecvTransport.h"
#import "RTCPeerConnectionFactory.h"
#include "libmediasoupclient/include/Transport.hpp"

@interface MSRecvTransport()
-(instancetype)initWithNativeTransport:(mediasoupclient::RecvTransport *)nativeTransport factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory;

@property(nonatomic, readonly) mediasoupclient::RecvTransport *recvTransport;

@end