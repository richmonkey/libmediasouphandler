package org.mediasoup;

public class Transport {
    protected long nativeTransport;
    
    public Transport(long transport) {
        nativeTransport = transport;
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

    private static native void nativeClose(long nativeTransport);
    private static native boolean nativeIsClosed(long nativeTransport);        
    private static native Fingerprint nativeGetFingerprint(long nativeTransport);
}
