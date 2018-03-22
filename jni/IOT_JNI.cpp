#include "IOT_ClientJNI.h"
#include "utils/Log.h"
#include "android_runtime/AndroidRuntime.h"

static const char* kClassPathName = "com/android/leiot/DeviceShadow";

using namespace android;

static void __native_init(JNIEnv *env)
{
    ALOGI("__native_init\n");
}

static void __native_setup(JNIEnv *env, jobject thiz, jobject refObj)
{
    ALOGI("__native_setup\n");
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        ALOGE("Can't find %s\n", kClassPathName);
        return;
    }
    IOTClientJNI::get().init(env, clazz, refObj);
}

static jint __native_reportEvent(JNIEnv *env, jobject thiz, jstring msg)
{
    return IOTClientJNI::get().reportEvent(msg);
}

static jint __native_reportProperty(JNIEnv *env, jobject thiz, jstring key, jstring val)
{
    return IOTClientJNI::get().reportProperty(key, val);
}

static JNINativeMethod gMethods[] = {
    { "native_init",            "()V",                            (void *)__native_init },
    { "native_setup",           "(Ljava/lang/Object;)V",          (void *)__native_setup },
    {
        "native_reportEvent",
        "(Ljava/lang/String;)I",
        (void *)__native_reportEvent
    },
    {
        "native_reportProperty",
        "(Ljava/lang/String;Ljava/lang/String;)I",
        (void *)__native_reportProperty
    },
};


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    ALOGI("JNI_OnLoad\n");
    JNIEnv* env = 0;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;
    jclass clazz = 0;
    if (!(clazz = env->FindClass(kClassPathName))) {
        ALOGE("JNI_OnLoad %s ERR\n", kClassPathName);
        return JNI_ERR;
    }
    env->RegisterNatives(clazz, gMethods, NELEM(gMethods));
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    ALOGI("JNI_Unload\n");
}
