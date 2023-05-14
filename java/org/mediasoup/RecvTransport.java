package org.mediasoup;
import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;
import org.webrtc.MediaStreamTrack;
import org.webrtc.RtpReceiver;

public class RecvTransport extends Transport {
    public static class RecvResult {
        public String localId;
        public MediaStreamTrack track;
        public RtpReceiver rtpReceiver;

        public RecvResult(String localId, RtpReceiver rtpReceiver, long nativeTrack) {
            try {
                Method method = MediaStreamTrack.class.getDeclaredMethod("createMediaStreamTrack", long.class);
                method.setAccessible(true);
                this.track = (MediaStreamTrack)method.invoke(null, nativeTrack);
                this.localId = localId;
                this.rtpReceiver = rtpReceiver;
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            }
        }
    }

    public RecvTransport(long nativeTransport) {
        super(nativeTransport);
    }

    public RecvResult consume(String id, String producerId, String kind, String rtpParameters) {
        return nativeConsume(nativeTransport, id, producerId, kind, rtpParameters);
    }

    private static native RecvResult nativeConsume(long nativeTransport, String id, String producerId, String kind, String rtpParameters);
}
