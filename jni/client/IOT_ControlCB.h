#ifndef __IOT_ControlCB_H_
#define __IOT_ControlCB_H_

#include <JNIHelp.h>
#include "IIOTControlCB.h"

namespace android {

class IOTControlCB : public BnIOTControlCB {
public:
    IOTControlCB(JNIEnv* env, jclass clazz, jobject thiz);
    ~IOTControlCB();
protected:
    int callback(String8 const &msg) const;
private:
    jclass m_class;
    jobject m_object;
    jmethodID m_controlCB;
};

} /* end namespace android */

#endif
