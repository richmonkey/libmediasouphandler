package org.mediasoup;

public class Transport {
    protected long nativeTransport;
    
    public Transport(long transport) {
        nativeTransport = transport;
    }

    public String getId() {
        return nativeGetId(nativeTransport);
    }
    
    public void close() {
        if (nativeTransport != 0) {
            nativeClose(nativeTransport);
            nativeTransport = 0;
        }
    }

    public boolean isClosed() {
        return nativeIsClosed(nativeTransport);
    }

    public Fingerprint getFingerprint() {
        return nativeGetFingerprint(nativeTransport);
    }

    public String getStats() {
        return nativeGetStats(nativeTransport);
    }

    public void restartIce(String iceParameters) {
        nativeRestartIce(nativeTransport, iceParameters);
    }

    private static native String nativeGetId(long nativeTransport);
    private static native void nativeClose(long nativeTransport);
    private static native boolean nativeIsClosed(long nativeTransport);        
    private static native Fingerprint nativeGetFingerprint(long nativeTransport);
    private static native String nativeGetStats(long nativeTransport);
    private static native void nativeRestartIce(long nativeTransport, String iceParameters);
    private static native void nativeUpdateIceServers(long nativeTransport, String iceServers);
}
