package org.mediasoup;

public class MediaSoupClient {
    private static native void initialize();
    private static native void cleanup();
    public static native String version();
}
