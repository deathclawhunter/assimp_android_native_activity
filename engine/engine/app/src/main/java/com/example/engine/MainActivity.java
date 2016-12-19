package com.example.engine;

import android.app.NativeActivity;

public class MainActivity extends NativeActivity {

    static {
        System.loadLibrary("stlport_shared");
        System.loadLibrary("assimp");
    }
}
