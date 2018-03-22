#include "IOT_Service.h"
#include <utils/Log.h>

namespace android {

IOTService::IOTService ()
{
}

IOTService::~IOTService ()
{
    m_clients.clear();
}

int IOTService::callControlCB(const char* data, size_t size)
{
    if (!data || size <= 0)
        return -1;
    for (size_t i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i]->m_control != NULL)
            m_clients[i]->m_control->callback(String8(data));
    }
    return 0;
}

int IOTService::callPropertyGet(const char *key, char *val, size_t size)
{
    if (!key || !val || size <= 0)
        return -1;
    int ret = -1;
    size_t len = 0;
    String8 str;
    /* TODO */
    for (size_t i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i]->m_property == NULL)
            continue;
        ret = m_clients[i]->m_property->get(String8(key), str);
        len = str.length();
        if (len == 0)
            continue;
        if (len > size) {
            ret = -2;
            len = size;
        }
        memcpy(val, str.string(), len);
        break;
    }
    return len;
}

int IOTService::callPropertySet(const char *key, const char *val)
{
    if (!key || !val)
        return -1;

    int ret = -1;
    size_t len = 0;
    String8 str;
    /* TODO */
    for (size_t i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i]->m_property == NULL)
            continue;
        ret = m_clients[i]->m_property->set(String8(key), String8(val));
        if (ret < 0)
            continue;
        break;
    }
    return ret;
}

sp<IIOTClient> IOTService::createClient()
{
    ALOGW("IOTService createClien");
    int cnt = m_clients.size();
    if (cnt >= MAX_CLINETS) {
        ALOGW("Client count beyond the max value[%d]\n", MAX_CLINETS);
        return NULL;
    }
    Mutex::Autolock _l(m_lock);
    sp<IOTClient> client = new IOTClient(this);
    m_clients.add(client);
    return client;
}

void IOTService::binderDied(const wp<IBinder>& cli)
{
    ALOGW("IOTService binderDied [%p]\n", cli.unsafe_get());
    Mutex::Autolock _l(m_lock);
    for (size_t i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i].get() == cli.unsafe_get()) {
            ALOGI("remove [%d]\n", i);
            m_clients.removeAt(i);
            return;
        }
    }
}

void IOTService::waitForClient()
{
    ALOGI("waitForClient()\n");
    Mutex::Autolock _l(m_lock);
    m_condition.wait(m_lock);
}

void IOTService::clientActive()
{
    ALOGI("clientActive()\n");
    Mutex::Autolock _l(m_lock);
    m_condition.signal();
}

int IOTService::IOTClient::reportEvent(String8 &msg)
{
    return m_service->doReportEvent(msg.string(), msg.length());
}

int IOTService::IOTClient::reportProperty(String8 &key, String8 &val)
{
    return m_service->doReportProperty(key.string(), val.string());
}

int IOTService::IOTClient::registControlCB(const sp<IIOTControlCB> &ctlCB)
{
    m_control = ctlCB;
    IInterface::asBinder(m_control)->linkToDeath(m_service);
    return 0;
}

int IOTService::IOTClient::registPropertyCB(const sp<IIOTPropertyCB> &proCB)
{
    m_property = proCB;
    IInterface::asBinder(m_property)->linkToDeath(m_service);
    m_service->clientActive();
    return 0;
}

} /* end namespace android */
