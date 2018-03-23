#include "IOT_ClientJNI.h"
#include "IOT_CommandCB.h"
#include "IOT_PropertyCB.h"
#include "IOT_Helper.h"

#include <binder/IServiceManager.h>

namespace android {

ANDROID_SINGLETON_STATIC_INSTANCE(IOTClientJNI)

IOTClientJNI::IOTClientJNI()
    : m_commandCB(0), m_propertyCB(0), m_proxy(0), m_listen(0)
{
}

IOTClientJNI::~IOTClientJNI()
{
    m_proxy.clear();
    m_listen.clear();
}

int IOTClientJNI::init(JNIEnv* env, jclass clazz, jobject thiz)
{
    m_commandCB = new IOTCommandCB(env, clazz, thiz);
    m_propertyCB = new IOTPropertyCB(env, clazz, thiz);
    return connect();
}

void IOTClientJNI::commit()
{
    m_proxy->registCommandCB(m_commandCB);
    m_proxy->registPropertyCB(m_propertyCB);
}

int IOTClientJNI::connect()
{
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder;

    /* wait iot service start, try 5 times */
    for (int i = 0; i < 5; ++i) {
        binder = sm->getService(String16(IOT_SERVICE_NAME));
        if(binder == 0) {
            usleep(200000);
            continue;
        }
        break;
    }
    if(binder == 0) {
        ALOGE("sm->getService(%s) error!\n", IOT_SERVICE_NAME);
        return -1;
    }
    sp<IIOTService> service = interface_cast<IIOTService>(binder);

    /* iot service die, call serviceDied for reconnect */
    class DeathObserver : public IBinder::DeathRecipient {
        IOTClientJNI& m_client;
        virtual void binderDied(const wp<IBinder>& who) {
            ALOGW("IIOTService died [%p]", who.unsafe_get());
            m_client.serviceDied();
        }
    public:
        DeathObserver(IOTClientJNI& c) : m_client(c) { }
    };
    m_listen = new DeathObserver(*const_cast<IOTClientJNI *>(this));
    IInterface::asBinder(service)->linkToDeath(m_listen);

    m_proxy = service->createClient(String8("todo"));
    return 0;
}

void IOTClientJNI::serviceDied()
{
    m_proxy.clear();
    m_listen.clear();
    int ret = connect();
    if (ret < 0)
        ALOGW("IOTClientJNI connect iot service fail!");
}

int IOTClientJNI::followProperty(jstring &jkey, jint type, jint size)
{
    if (m_proxy == NULL)
        return -1;
    String8 key = JStringToString8(jkey);
    if (key.length() <= 0)
        return -1;
    ALOGI("followProperty(%s)", key.string());
    return m_proxy->followProperty(key, type, size);
}

int IOTClientJNI::followCommand(jstring &jcmd)
{
    if (m_proxy == NULL)
        return -1;
    String8 cmd = JStringToString8(jcmd);
    if (cmd.length() <= 0)
        return -1;
    ALOGI("followCommand(%s)", cmd.string());
    return m_proxy->followCommand(cmd);
}

int IOTClientJNI::reportEvent(jstring const &jmsg)
{
    if (m_proxy == NULL)
        return -1;
    String8 msg = JStringToString8(jmsg);
    if (msg.length() <= 0)
        return -1;
    ALOGI("reportEvent(%s)", msg.string());
    return m_proxy->reportEvent(msg);
}

int IOTClientJNI::reportProperty(jstring const &jkey, jstring const &jval)
{
    if (m_proxy == NULL)
        return -1;
    String8 key = JStringToString8(jkey);
    if (key.length() <= 0)
        return -1;
    String8 val = JStringToString8(jval);
    if (val.length() <= 0)
        return -1;
    ALOGI("reportProperty(%s, %s", key.string(), val.string());
    return m_proxy->reportProperty(key, val);
}

} /* end namespace android */

