package com.android.leiot;

import android.content.Context;
import android.util.Log;
import java.lang.ref.WeakReference;

abstract public class DeviceShadow {
    private static final String TAG = "DeviceShadow";

    static {
        System.loadLibrary("iotclient_jni");
        native_init();
    }

    private static native final void native_init();
    private native final void native_setup(Object refobj);

    protected native int native_reportEvent(String msg);
    protected native int native_reportProperty(String key, String val);

    protected DeviceShadow() {
        Log.d(TAG, "DeviceShadow");
        native_setup(new WeakReference<DeviceShadow>(this));
    }

    protected abstract void iotControlCallback(String data);
    protected abstract String iotPropertyGet(String key);
    protected abstract void iotPropertySet(String key, String val);

    private static void _iotControlCallback(Object o, String data) {
        DeviceShadow shadow = (DeviceShadow)((WeakReference)o).get();  
        if (shadow == null)
            return;
        shadow.iotControlCallback(data);
    }

    private static String _iotPropertyGet(Object o, String key) {
        DeviceShadow shadow = (DeviceShadow)((WeakReference)o).get();  
        if (shadow == null)
            return null;
        return shadow.iotPropertyGet(key);
    }

    private static void _iotPropertySet(Object o, String key, String val) {
        DeviceShadow shadow = (DeviceShadow)((WeakReference)o).get();  
        if (shadow == null)
            return ;
        shadow.iotPropertySet(key, val);
    }
}
