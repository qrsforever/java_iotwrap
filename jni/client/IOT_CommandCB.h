#ifndef __IOT_CommandCB_H_
#define __IOT_CommandCB_H_

#include <JNIHelp.h>
#include "IIOTCommandCB.h"

namespace android {

class IOTCommandCB : public BnIOTCommandCB {
public:
    IOTCommandCB(JNIEnv* env, jclass clazz, jobject thiz);
    ~IOTCommandCB();
protected:
    int callback(String8 const &cmd, String8 const &msg) const;
private:
    jclass m_class;
    jobject m_object;
    jmethodID m_controlCB;
};

} /* end namespace android */

#endif
