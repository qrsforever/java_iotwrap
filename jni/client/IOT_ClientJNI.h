#ifndef __IOT_ClientJNI_H_
#define __IOT_ClientJNI_H_

#include <utils/Singleton.h>
#include <JNIHelp.h>

#include "IIOTService.h"

namespace android {

class IOTClientJNI : public Singleton<IOTClientJNI> {
private:
    friend class Singleton<IOTClientJNI>;
    IOTClientJNI();

    int connect();
    void serviceDied();

public:
    ~IOTClientJNI();
    static inline IOTClientJNI& get() { return getInstance(); }

    int init(JNIEnv* env, jclass clazz, jobject thiz);

    int reportEvent(jstring const& msg);
    int reportProperty(jstring const& key, jstring const& val);

private:
    sp<IIOTControlCB> m_controlCB;
    sp<IIOTPropertyCB> m_propertyCB;
    mutable sp<IIOTClient> m_proxy;
    mutable sp<IBinder::DeathRecipient> m_listen;
};

} /* end namespace android */

#endif
