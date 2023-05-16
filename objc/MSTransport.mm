#import "MSTransport.h"
#import "MSTransport+Private.h"

@implementation MSFingerprint

-(instancetype)initWithAlgorithm:(NSString*)algorithm fingerprint:(NSString*)fingerprint {
    self = [super init];
    if (self) {
        self.algorithm = algorithm;
        self.fingerprint = fingerprint;
    }
    return self;
}

@end

@implementation MSTransport {
    // use std::unique_ptr? 
    // https://stackoverflow.com/questions/3135782/destructor-in-objective-c
    mediasoupclient::Transport *_transport;
}

-(instancetype)initWithNativeTransport:(mediasoupclient::Transport *)nativeTransport factory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory {
    self = [super init];
    if (self) {
        _transport = nativeTransport;
        self.factory = factory;
    }
    return self;
}

-(void)dealloc {
    delete _transport;
}


-(NSString*)transportId {
    std::string _id = _transport->GetId();
    return [NSString stringWithUTF8String:_id.c_str()];
}

-(mediasoupclient::Transport*)nativeTransport {
    return _transport;
}

-(MSFingerprint*)fingerprint {
    std::string algorithm;
    std::string fingerprint;
    self.nativeTransport->GetFingerprint(algorithm, fingerprint);
    return [[MSFingerprint alloc] initWithAlgorithm:[NSString stringWithUTF8String:algorithm.c_str()] fingerprint:[NSString stringWithUTF8String:fingerprint.c_str()]];
}

-(void)close {
    _transport->Close();
}

-(NSString*)getStats {
    auto stats = _transport->GetStats();
    auto statsDump = stats.dump();
    return [NSString stringWithUTF8String:statsDump.c_str()];
}

-(void)restartIce:(NSString*)iceParameters {
    auto iceParametersJson = nlohmann::json::parse([iceParameters UTF8String]);
    _transport->RestartIce(iceParametersJson);
}

@end