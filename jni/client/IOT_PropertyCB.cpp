#include "IOT_PropertyCB.h"
#include "IOT_Helper.h"

#include <utils/Log.h>
#include <android_runtime/AndroidRuntime.h>

namespace android {

IOTPropertyCB::IOTPropertyCB(JNIEnv* env, jclass clazz, jobject thiz)
    : m_class(0), m_object(0), m_property_get(0), m_property_set(0)
{
    m_property_get = env->GetStaticMethodID(
        clazz,
        "_iotPropertyGet",
        "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/String;");
    if (!m_property_get) {
        ALOGE("GetStaticMethodID(iotPropertyCallback) error!\n");
        return;
    }

    m_property_set = env->GetStaticMethodID(
        clazz,
        "_iotPropertySet",
        "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;)V");
    if (!m_property_set) {
        ALOGE("GetStaticMethodID(iotPropertyCallback) error!\n");
        return;
    }

    m_class = (jclass)env->NewGlobalRef(clazz);
    m_object = env->NewGlobalRef(thiz);
}

IOTPropertyCB::~IOTPropertyCB()
{
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (env) {
        env->DeleteGlobalRef(m_object);
        env->DeleteGlobalRef(m_class);
    }
}

int IOTPropertyCB::get(String8 const &key, String8 &val) const
{
    ALOGI("getProperty(%s)\n", key.string());
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    jstring jkey = env->NewStringUTF(key.string());
    if (env && jkey) {
        jstring jval = (jstring)env->CallStaticObjectMethod(
            m_class, m_property_get,
            m_object, jkey);
        if (jval) {
            val = JStringToString8(jval);
            env->DeleteLocalRef(jval);
        }
        env->DeleteLocalRef(jkey);
        return 0;
    }
    return -1;
}

int IOTPropertyCB::set(String8 const &key, String8 const &val) const
{
    ALOGI("setProperty(%s, %s)\n", key.string(), val.string());
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    jstring jkey = env->NewStringUTF(key.string());
    jstring jval = env->NewStringUTF(val.string());
    if (env && jkey && jval) {
        env->CallStaticVoidMethod(
            m_class, m_property_get,
            m_object, jkey, jval);
        env->DeleteLocalRef(jkey);
        env->DeleteLocalRef(jval);
        return 0;
    }
    return -1;
}

} /* end namespace android */
