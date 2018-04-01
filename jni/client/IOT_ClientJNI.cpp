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
    /* never run here */
    m_proxy.clear();
    m_listen.clear();
    for (size_t i = 0; i < m_propertyList.size(); ++i)
        delete m_propertyList[i];
    for (size_t i = 0; i < m_commandList.size(); ++i)
        delete m_commandList[i];
    m_propertyList.clear();
    m_commandList.clear();
}

int IOTClientJNI::init(JNIEnv* env, jclass clazz, jobject thiz, jstring &clientID)
{
    m_clientID = JStringToString8(clientID);
    m_propertyCB = new IOTPropertyCB(env, clazz, thiz);
    m_commandCB = new IOTCommandCB(env, clazz, thiz);
    return ERR_NOERROR;
}

int IOTClientJNI::connService(jint timeout_ms)
{
    if (m_proxy != NULL) {
        ALOGW(" Already connect!\n");
        return ERR_NOERROR;
    }

    int ret = _connect(timeout_ms);
    if (ret < 0) {
        ALOGW("IOTClientJNI connect iot service fail!");
        return ret;
    }

    /* set follow property and command to iot server */
    return _postFollow();
}

int IOTClientJNI::_postFollow()
{
    int ret = -1;
    if (m_proxy == NULL) {
        ALOGE(" Client proxy is null.\n");
        return ERR_PROXY_NULL;
    }

    {
        Mutex::Autolock _l(m_lockPropertyList);
        for (size_t i = 0; i < m_propertyList.size(); ++i) {
            String8 &key = m_propertyList[i]->_key;
            int type = m_propertyList[i]->_type;
            int size = m_propertyList[i]->_size;
            ret = m_proxy->followProperty(key, type, size);
            if (ret < 0) {
                ALOGW(" proxy->followProperty(%s,%d,%d) is error!.\n", key.string(), type, size);
            }
        }
    }

    {
        Mutex::Autolock _l(m_lockCommandList);
        for (size_t i = 0; i < m_commandList.size(); ++i) {
            String8 &cmd = m_commandList[i]->_cmd;
            m_proxy->followCommand(cmd);
            if (ret < 0) {
                ALOGW(" proxy->followCommand(%s) is error!.\n", cmd.string());
            }
        }
    }

    Mutex::Autolock _l(m_lockProxy);
    /* must put it after proxy->followxxx() */
    m_proxy->registCommandCB(m_commandCB);
    m_proxy->registPropertyCB(m_propertyCB);

    return ERR_NOERROR;
}

int IOTClientJNI::_connect(jint timeout_ms)
{
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder;
    sp<IIOTService> service = NULL;
    int status = DS_STATUS_INVALID;

    /* wait iot service start, try n times */
    const int tryCnt = timeout_ms * 1000 / USLEEP_TIME + 1;
    for (int i = 0; i < tryCnt; ++i) {
        if (service == NULL) {
            binder = sm->getService(String16(IOT_SERVICE_NAME));
            if(binder == 0) {
                usleep(USLEEP_TIME);
                continue;
            }
            service = interface_cast<IIOTService>(binder);
        }
        status = service->getServiceStatus();
        if (status == DS_STATUS_CONNECTED)
            break;
        usleep(200000);
        continue;
    }
    if(status != DS_STATUS_CONNECTED) {
        ALOGE("getService(%s) or getStatus(%d) error!\n", IOT_SERVICE_NAME, status);
        return ERR_PROXY_NULL;
    }

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

    Mutex::Autolock _l(m_lockProxy);
    m_proxy = service->createClient(m_clientID);
    ALOGI("create Client [%p]\n", m_proxy.get());
    return m_proxy == NULL ? ERR_PROXY_NULL : ERR_NOERROR;
}

void IOTClientJNI::serviceDied()
{
    m_proxy.clear();
    m_listen.clear();
    m_proxy = NULL;
    m_listen = NULL;
    int ret = connService(USLEEP_TIME * 60);
    ALOGI("Auto connect service result:%d\n", ret);
}

int IOTClientJNI::followProperty(jstring &jkey, jint type, jint size)
{
    String8 key = JStringToString8(jkey);
    if (key.length() <= 0)
        return ERR_PARAM_INVALID;
    ALOGI("followProperty(%s)", key.string());
    Mutex::Autolock _l(m_lockPropertyList);
    for (size_t i = 0; i < m_propertyList.size(); ++i) {
        if (m_propertyList[i]->isKeyEqual(key)) {
            ALOGW("Already add followProperty(%s)", key.string());
            return ERR_PARAM_INVALID;
        }
    }
    m_propertyList.add(new _PropertyInfo(key, type, size));
    return ERR_NOERROR;
}

int IOTClientJNI::followCommand(jstring &jcmd)
{
    String8 cmd = JStringToString8(jcmd);
    if (cmd.length() <= 0)
        return -1;
    ALOGI("followCommand(%s)", cmd.string());
    Mutex::Autolock _l(m_lockCommandList);
    for (size_t i = 0; i < m_commandList.size(); ++i) {
        if (m_commandList[i]->isCmdEqual(cmd)) {
            ALOGW("Already add followCommand(%s)", cmd.string());
            return ERR_PARAM_INVALID;
        }
    }
    m_commandList.add(new _CommandInfo(cmd));
    return ERR_NOERROR;
}

int IOTClientJNI::reportEvent(jstring const &jmsg)
{
    if (m_proxy == NULL)
        return ERR_PROXY_NULL;

    String8 msg = JStringToString8(jmsg);
    if (msg.length() <= 0)
        return ERR_PARAM_INVALID;
    ALOGI("reportEvent(%s)", msg.string());

    Mutex::Autolock _l(m_lockProxy);
    return m_proxy->reportEvent(msg);
}

int IOTClientJNI::reportProperty(jstring const &jkey, jstring const &jval)
{
    if (m_proxy == NULL)
        return ERR_PROXY_NULL;

    String8 key = JStringToString8(jkey);
    if (key.length() <= 0)
        return ERR_PARAM_INVALID;
    String8 val = JStringToString8(jval);
    if (val.length() <= 0)
        return ERR_PARAM_INVALID;
    ALOGI("reportProperty(%s, %s)", key.string(), val.string());
    Mutex::Autolock _l(m_lockProxy);
    return m_proxy->reportProperty(key, val);
}

} /* end namespace android */

