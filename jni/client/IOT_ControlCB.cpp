#include "IOT_ControlCB.h"
#include "IOT_Helper.h"

#include <utils/Log.h>
#include <android_runtime/AndroidRuntime.h>

namespace android {

IOTControlCB::IOTControlCB(JNIEnv* env, jclass clazz, jobject thiz)
    : m_class(0), m_object(0), m_controlCB(0)
{
    m_controlCB = env->GetStaticMethodID(
        clazz,
        "_iotControlCallback",
        "(Ljava/lang/Object;Ljava/lang/String;)V");

    if (!m_controlCB) {
        ALOGE("GetStaticMethodID(iotControlCallback) error!\n");
        // jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    m_class = (jclass)env->NewGlobalRef(clazz);
    m_object = env->NewGlobalRef(thiz);
}

IOTControlCB::~IOTControlCB()
{
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (env) {
        env->DeleteGlobalRef(m_object);
        env->DeleteGlobalRef(m_class);
    }
}

int IOTControlCB::callback(String8 const &msg) const
{
    ALOGI("callback(%s)\n", msg.string());
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    jstring jmsg = env->NewStringUTF(msg.string());
    if (jmsg) {
        env->CallStaticVoidMethod(
            m_class, m_controlCB,
            m_object, jmsg);
        env->DeleteLocalRef(jmsg);
        return 0;
    }
    return -1;
}

} /* end namespace android */
