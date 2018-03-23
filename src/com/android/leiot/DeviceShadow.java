package com.android.leiot;

import android.content.Context;
import android.util.Log;
import java.lang.ref.WeakReference;

abstract public class DeviceShadow {
    private static final String TAG = "DeviceShadow";


    public static final int IOT_PROPERTY_NULL   = 0;
    public static final int IOT_PROPERTY_INT32  = 1;
    public static final int IOT_PROPERTY_STRING = 2;

    static {
        System.loadLibrary("iotclient_jni");
        native_init();
    }

    private static native final void native_init();
    private native final void native_setup(Object refobj);

    protected native int native_followProperty(String key, int type, int size);
    protected native int native_followCommand(String cmd);
    protected native int native_reportEvent(String msg);
    protected native int native_reportProperty(String key, String val);
    protected native void native_commit();

    protected DeviceShadow() {
        Log.d(TAG, "DeviceShadow");
        native_setup(new WeakReference<DeviceShadow>(this));
    }

    protected int iotFollowProperty(String key, int type, int size) {
        Log.d(TAG, " iotFollowProperty(" + key + ", " + type + ", " + size + ")");
        return native_followProperty(key, type, size);
    }

    protected int iotFollowCommand(String cmd) {
        return native_followCommand(cmd);
    }

    protected int iotReportEvent(String msg) {
        return native_reportEvent(msg);
    }

    protected int iotReportProperty(String key, String val) {
        return native_reportProperty(key, val);
    }

    protected void iotCommit() {
        native_commit();
    }

    protected abstract void iotCommandCallback(String data);
    protected abstract String iotPropertyGet(String key);
    protected abstract void iotPropertySet(String key, String val);

    private static void _iotCommandCallback(Object o, String data) {
        DeviceShadow shadow = (DeviceShadow)((WeakReference)o).get();  
        if (shadow == null)
            return;
        shadow.iotCommandCallback(data);
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
