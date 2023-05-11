package org.mediasoup;
import org.webrtc.MediaStreamTrack;
import org.webrtc.RtpReceiver;

public class RecvTransport extends Transport {
    public static class RecvResult {
        public String localId;
        public MediaStreamTrack track;
        public RtpReceiver rtpReceiver;

        public RecvResult(String localId, RtpReceiver rtpReceiver, long nativeTrack) {
            Method method = MediaStreamTrack.class.getDeclaredMethod("createMediaStreamTrack", long.class, MediaStreamTrack.class);
            method.setAccessible(true);
            this.track = (MediaStreamTrack)method.invoke(MediaStreamTrack.class, nativeTrack);
            this.localId = localId;
            this.rtpReceiver = rtpReceiver;
            this.track = track;
        }
    }

    public RecvResult consume(String id, String producerId, String kind, String rtpParameters) {
        return nativeConsume(nativeTransport, id, producerId, kind, rtpParameters);
    }

    private static native RecvResult nativeConsume(long nativeTransport, String id, String producerId, String kind, String rtpParameters);
}
