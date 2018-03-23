#ifndef __IOT_ClientJNI_H_
#define __IOT_ClientJNI_H_

#include <utils/Singleton.h>
#include <JNIHelp.h>

#include "IIOTService.h"
#include <string>

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

    int init(JNIEnv* env, jclass clazz, jobject thiz, jstring &clientID);
    int connService();
    int postFollow();

    int followProperty(jstring &jkey, jint type, jint size);
    int followCommand(jstring &cmd);
    int reportEvent(jstring const& msg);
    int reportProperty(jstring const& key, jstring const& val);

private:
    String8 m_clientID;
    sp<IIOTCommandCB> m_commandCB;
    sp<IIOTPropertyCB> m_propertyCB;

    mutable sp<IIOTClient> m_proxy;
    mutable sp<IBinder::DeathRecipient> m_listen;
};

} /* end namespace android */

#endif
