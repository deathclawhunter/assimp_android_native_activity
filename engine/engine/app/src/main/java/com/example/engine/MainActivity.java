package com.example.engine;

import android.app.NativeActivity;

public class MainActivity extends NativeActivity {

    final static String TAG = "MainActivity";

    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("assimp");

        //ffmpeg - sequence matters
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("avfilter-6");
    }
}
