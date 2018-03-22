#ifndef __IOT_PropertyCB_H_
#define __IOT_PropertyCB_H_

#include <JNIHelp.h>
#include "IIOTPropertyCB.h"

namespace android {

class IOTPropertyCB : public BnIOTPropertyCB {
public:
    IOTPropertyCB(JNIEnv* env, jclass clazz, jobject thiz);
    ~IOTPropertyCB();

protected:
    int get(String8 const &key, String8 &val) const;
    int set(String8 const &key, String8 const &val) const;

private:
    jclass m_class;
    jobject m_object;
    jmethodID m_property_get;
    jmethodID m_property_set;
};

} /* end namespace android */

#endif
