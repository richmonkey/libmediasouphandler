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

    public SendTransport createSendTransport(SendTransport.Listener listener,
                                             String id,
                                             String iceParameters,
                                             String iceCandidates,
                                             String dtlsParameters) {

        SendTransport transport = new SendTransport();
        long nativeTransport = nativeCreateSendTransport(transport, listener, id,
            iceParameters, iceCandidates, dtlsParameters);

        transport.setNativeTransport(nativeTransport);
        return transport;
    }

    public RecvTransport createRecvTransport(RecvTransport.Listener listener,
                                             String id,
                                             String iceParameters,
                                             String iceCandidates,
                                             String dtlsParameters) {
        RecvTransport transport = new RecvTransport();
        long nativeTransport = nativeCreateRecvTransport(transport, listener, id,
            iceParameters, iceCandidates, dtlsParameters);
        transport.setNativeTransport(nativeTransport);
        return transport;
    }
    
    private static native long nativeCreateDevice();
    private static native void nativeLoad(long nativeDevice,
                                          String routerRtpCapabilities,
                                          Object rtcConfig,
                                          long nativeFactory);

    private static native boolean nativeIsLoaded(long nativeDevice);
    private static native void nativeRelease(long nativeDevice);

    private static native long nativeCreateSendTransport(SendTransport transport,
                                                         SendTransport.Listener listener,
                                                         String id,
                                                         String iceParameters,
                                                         String iceCandidates,
                                                         String dtlsParamters);

   private static native long nativeCreateRecvTransport(RecvTransport transport,
                                                        RecvTransport.Listener listener,
                                                        String id,
                                                        String iceParameters,
                                                        String iceCandidates,
                                                        String dtlsParamters);    
}
