package org.mediasoup;

import org.webrtc.PeerConnection;
import org.webrtc.PeerConnectionFactory;

public class Device {
    private long nativeDevice;
    
    public Device() {
        nativeDevice = nativeCreateDevice();
    }

    public void load(String routerRtpCapabilities,
                     PeerConnection.RTCConfiguration rtcConfig,
                     PeerConnectionFactory factory) {
        nativeLoad(nativeDevice, routerRtpCapabilities,
                   rtcConfig, factory.getNativePeerConnectionFactory());
    }

    public boolean isLoaded() {
        if (nativeDevice != 0) {
            return nativeIsLoaded(nativeDevice);
        }
        return false;
    }
    
    public void dispose() {
        if (nativeDevice != 0) {
            nativeRelease(nativeDevice);
            nativeDevice = 0;
        }
    }

    public String getRtpCapabilities() {
        return nativeGetRtpCapabilities(nativeDevice);
    }

    public String getSctpCapabilities() {
        return nativeGetSctpCapabilities(nativeDevice);
    }

    public SendTransport createSendTransport(String id,
                                             String iceParameters,
                                             String iceCandidates,
                                             String dtlsParameters,
                                             PeerConnection.RTCConfiguration rtcConfig,
                                             PeerConnectionFactory factory) {
        return nativeCreateSendTransport(nativeDevice, id, iceParameters, iceCandidates, dtlsParameters, rtcConfig, factory.getNativePeerConnectionFactory());
    }

    public RecvTransport createRecvTransport(String id,
                                             String iceParameters,
                                             String iceCandidates,
                                             String dtlsParameters,
                                             PeerConnection.RTCConfiguration rtcConfig,
                                             PeerConnectionFactory factory) {
        return nativeCreateRecvTransport(nativeDevice, id, iceParameters, iceCandidates, dtlsParameters, rtcConfig, factory.getNativePeerConnectionFactory());
    }
    
    private static native long nativeCreateDevice();
    private static native void nativeLoad(long nativeDevice,
                                          String routerRtpCapabilities,
                                          PeerConnection.RTCConfiguration rtcConfig,
                                          long nativeFactory);

    private static native boolean nativeIsLoaded(long nativeDevice);
    private static native void nativeRelease(long nativeDevice);

    private static native String nativeGetRtpCapabilities(long nativeDevice);
    private static native String nativeGetSctpCapabilities(long nativeDevice);

    private static native SendTransport nativeCreateSendTransport(long nativeDevice, 
                                                         String id,
                                                         String iceParameters,
                                                         String iceCandidates,
                                                         String dtlsParamters,
                                                         PeerConnection.RTCConfiguration rtcConfig,
                                                         long nativeFactory);

   private static native RecvTransport nativeCreateRecvTransport(long nativeDevice, 
                                                        String id,
                                                        String iceParameters,
                                                        String iceCandidates,
                                                        String dtlsParamters,
                                                        PeerConnection.RTCConfiguration rtcConfig,
                                                        long nativeFactory);    
}
