#ifndef __IOT_HELPER_H_
#define __IOT_HELPER_H_

#include <JNIHelp.h>
#include <utils/String8.h>

namespace android {

String8 JStringToString8(jstring const &jstr);

}

#endif
