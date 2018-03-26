#ifndef __IOT_ClientJNI_H_
#define __IOT_ClientJNI_H_

#include <utils/Singleton.h>
#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <JNIHelp.h>

#include "IIOTService.h"
#include <string>

enum {
    ERR_NOERROR = 0,
    ERR_PROXY_NULL = -1,
    ERR_PARAM_INVALID = -2,
};

#define TRY_CONNECT_SERVICE_CNT 20

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
    int reconnService();
    int postFollow();

    int followProperty(jstring &jkey, jint type, jint size);
    int followCommand(jstring &cmd);
    int reportEvent(jstring const& msg);
    int reportProperty(jstring const& key, jstring const& val);

private:
    String8 m_clientID;

    struct _PropertyInfo {
        _PropertyInfo(String8 key, int type, int size):
            _key(key), _type(type), _size(size) {}
        bool isKeyEqual(const String8 &key) {
            return key == _key;
        }
        String8 _key;
        int _type;
        int _size;
    };

    struct _CommandInfo {
        _CommandInfo(String8 cmd): _cmd(cmd) {}
        bool isCmdEqual(const String8 &cmd) {
            return cmd == _cmd;
        }
        String8 _cmd;
    };

    /* If iot_server died, then iot server restart itself, need reset those info to it */
    Vector<_PropertyInfo*> m_propertyList;
    Vector<_CommandInfo*> m_commandList;

    sp<IIOTCommandCB> m_commandCB;
    sp<IIOTPropertyCB> m_propertyCB;

    Mutex m_lockPropertyList;
    Mutex m_lockCommandList;
    Mutex m_lockProxy;
    mutable sp<IIOTClient> m_proxy;
    mutable sp<IBinder::DeathRecipient> m_listen;
};

} /* end namespace android */

#endif
