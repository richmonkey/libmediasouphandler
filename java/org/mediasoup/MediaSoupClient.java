package org.mediasoup;

public class MediaSoupClient {
    public static native void initialize();
    public static native void cleanup();
    public static native String version();
}
