
#import "MSClient.h"
#include "libmediasoupclient/include/mediasoupclient.hpp"
#include "libmediasoupclient/include/Logger.hpp"
#include <memory>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>

namespace webrtc {
    //make link works
    std::unique_ptr<webrtc::VideoDecoderFactory> CreateBuiltinVideoDecoderFactory() {
        return nullptr;
    }

    std::unique_ptr<webrtc::VideoEncoderFactory> CreateBuiltinVideoEncoderFactory() {
        return nullptr;
    }

}

class IosLogHandler : public mediasoupclient::Logger::LogHandlerInterface {
    void OnLog(mediasoupclient::Logger::LogLevel level, char* payload, size_t len) {
        NSLog(@"%s", payload);
    }
};



@implementation MSClient {

}

+(void)initialize {
    if (self == [MSClient self]) {
        mediasoupclient::Logger::SetHandler(new IosLogHandler());
        mediasoupclient::Logger::SetLogLevel(mediasoupclient::Logger::LogLevel::LOG_DEBUG);
        NSLog(@"MSClient initialize");
    }
}

+(void)cleanup {
    NSLog(@"MSClient cleanup");
}

+(NSString*)version {
    std::string version = mediasoupclient::Version();
    return [NSString stringWithUTF8String:version.c_str()];
}


@end
