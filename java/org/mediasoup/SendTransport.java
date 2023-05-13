package org.mediasoup;
import org.webrtc.RtpParameters;
import org.webrtc.MediaStreamTrack;
import org.webrtc.RtpSender;

public class SendTransport extends Transport {
    public static class SendResult {
        public String localId;
        public RtpSender rtpSender;
        public String rtpParameters;//json object as org.webrtc.RtpParameters

        public SendResult(String localId, RtpSender rtpSender, String rtpParameters) {
            this.localId = localId;
            this.rtpSender = rtpSender;
            this.rtpParameters = rtpParameters;
        }
    }

    public SendTransport(long nativeTransport) {
        super(nativeTransport);
    }
    
    public SendResult produce(MediaStreamTrack track, RtpParameters.Encoding[] encodings, String codecOptions, String codec) {
        if (encodings == null) {
            encodings = new RtpParameters.Encoding[0];
        }
        if (codecOptions == null) {
            codecOptions = "{}";
        }
        if (codec == null) {
            codec = "{}";
        }
        SendResult sendResult = nativeProduce(nativeTransport, track, encodings, codecOptions, codec);
        return sendResult;
    }

    private static native SendResult nativeProduce(long nativeTransport,
                                             MediaStreamTrack track,
                                             RtpParameters.Encoding[] encodings,
                                             String codecOptions,
                                             String codec);
}


