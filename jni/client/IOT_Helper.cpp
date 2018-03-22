#include "IOT_Helper.h"
#include <android_runtime/AndroidRuntime.h>

namespace android {

String8 JStringToString8(jstring const &jstr)
{
    String8 result;
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (env) {
        const char *s = env->GetStringUTFChars(jstr, 0);
        if (s) {
            result = s;
            env->ReleaseStringUTFChars(jstr, s);
        }
    }
    return result;
}

}
