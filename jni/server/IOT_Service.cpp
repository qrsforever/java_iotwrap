#include "IOT_Service.h"
#include <utils/Log.h>

#include <utils/Vector.h>

namespace android {

IOTService::IOTService ()
{
}

IOTService::~IOTService ()
{
    /* TODO never run here, release resource */
    IterFollow keyit = m_keyFollow.begin();
    for (; keyit != m_keyFollow.end(); ++keyit) {
        if (keyit->second) {
            keyit->second->clear();
            delete keyit->second;
        }
    }
    m_keyFollow.clear();

    IterFollow cmdit = m_cmdFollow.begin();
    for (; cmdit != m_cmdFollow.end(); ++cmdit) {
        if (cmdit->second) {
            cmdit->second->clear();
            delete cmdit->second;
        }
    }
    m_cmdFollow.clear();

    m_clients.clear();
}

int IOTService::callCommandCB(const char *cmd, const char* data)
{
    if (!cmd || !data)
        return -1;
    Mutex::Autolock _l(m_lockClis);

    IterFollow it = m_cmdFollow.find(cmd);
    if (it == m_cmdFollow.end()) {
        /* TODO */
        ALOGW("TODO callCommandCB, if no following, just using the first client.\n");
        if (m_clients.size() > 0 && m_clients[0] != NULL) {
            if (m_clients[0]->m_command != NULL)
                m_clients[0]->m_command->callback(String8(cmd), String8(data));
            return 0;
        }
        return -1;
    }
    Vector<sp<IOTClient>> *v_clients = it->second;
    if (!v_clients)
        return -1;

    for (size_t i = 0; i < v_clients->size(); ++i) {
        if ((*v_clients)[i]->m_command == NULL)
            continue;
        (*v_clients)[i]->m_command->callback(String8(cmd), String8(data));
    }
    return 0;
}

int IOTService::callPropertyGet(const char *key, char *val, size_t size)
{
    if (!key || !val || size <= 0)
        return -1;

    Mutex::Autolock _l(m_lockClis);

    ALOGI("callPropertyGet(%s)\n", key);
    size_t len = 0;
    String8 str;
    IterFollow it = m_keyFollow.find(key);
    if (it == m_keyFollow.end()) {
        /* TODO */
        ALOGW("TODO callPropertyGet, if no following, just using the first client.\n");
        if (m_clients.size() > 0 && m_clients[0] != NULL) {
            if (m_clients[0]->m_property != NULL) {
                m_clients[0]->m_property->get(String8(key), str);
                len = str.length();
                if (len > 0) {
                    if (len > size) {
                        len = size;
                    }
                    memcpy(val, str.string(), len);
                }
            }
            return 0;
        }
        return -1;
    }
    Vector<sp<IOTClient>> *v_clients = it->second;
    if (!v_clients)
        return -1;

    /* TODO only the first return, then terminate */
    for (size_t i = 0; i < v_clients->size(); ++i) {
        if ((*v_clients)[i]->m_property == NULL)
            continue;
        (*v_clients)[i]->m_property->get(String8(key), str);
        len = str.length();
        if (len == 0)
            continue;
        if (len > size) {
            len = size;
        }
        memcpy(val, str.string(), len);
    }
    return len;
}

int IOTService::callPropertySet(const char *key, const char *val)
{
    if (!key || !val)
        return -1;

    Mutex::Autolock _l(m_lockClis);

    ALOGI("callPropertySet(%s %s)\n", key, val);
    IterFollow it = m_keyFollow.find(key);
    if (it == m_keyFollow.end()) {
        /* TODO */
        ALOGW("TODO callPropertySet(%s), if no following, just using the first client.\n", key);
        if (m_clients.size() > 0 && m_clients[0] != NULL) {
            if (m_clients[0]->m_property != NULL)
                m_clients[0]->m_property->set(String8(key), String8(val));
            return 0;
        }
        return -1;
    }
    Vector<sp<IOTClient>> *v_clients = it->second;
    if (!v_clients)
        return -1;

    for (size_t i = 0; i < v_clients->size(); ++i) {
        if ((*v_clients)[i]->m_property == NULL)
            continue;

        ALOGI("m_property(%s %s)\n", key, val);
        (*v_clients)[i]->m_property->set(String8(key), String8(val));
    }
    return 0;
}

sp<IIOTClient> IOTService::createClient(String8 name)
{
    int cnt = m_clients.size();
    if (cnt >= MAX_CLINETS) {
        ALOGW("Client count beyond the max value[%d]\n", MAX_CLINETS);
        return NULL;
    }
    Mutex::Autolock _l(m_lockClis);
    ALOGI("IOTService add client [%s]\n", name.string());

    sp<IOTClient> client = new IOTClient(name.string(), this);
    m_clients.add(client);
    return client;
}

int IOTService::removeClient(const sp<IIOTClient> &client)
{
    Mutex::Autolock _l(m_lockClis);

    int rmflg = 0;

    IterFollow keyit = m_keyFollow.begin();
    for (; keyit != m_keyFollow.end(); ++keyit) {
        Vector<sp<IOTClient>> *v_clients = keyit->second;
        if (!v_clients)
            continue;
        for (size_t i = 0, rmflg = 0; rmflg == 0 && i < v_clients->size(); ++i) {
            if ((*v_clients)[i] == client) {
                v_clients->removeAt(i);
                rmflg = 1;
            }
        }
    }

    IterFollow cmdit = m_cmdFollow.begin();
    for (; cmdit != m_cmdFollow.end(); ++cmdit) {
        Vector<sp<IOTClient>> *v_clients = cmdit->second;
        if (!v_clients)
            continue;
        for (size_t i = 0, rmflg = 0; rmflg == 0 && i < v_clients->size(); ++i) {
            if ((*v_clients)[i] == client) {
                v_clients->removeAt(i);
                rmflg = 1;
            }
        }
    }

    for (size_t i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i] == client) {
            m_clients.removeAt(i);
            return 1;
        }
    }
    return 0;
}

int IOTService::addFollowing(int type, const char *val, const sp<IOTClient> &target)
{
    if (!val)
        return -1;
    Mutex::Autolock _l(m_lockClis);
    IterFollow it;
    Vector<sp<IOTClient>> *v_clients = 0;
    switch (type) {
    case FOLLOW_TYPE_PROPERTY:
        {
            it = m_keyFollow.find(val);
            if (it == m_keyFollow.end()) {
                v_clients = new Vector<sp<IOTClient>>();
                m_keyFollow.insert(
                    std::pair<std::string, Vector<sp<IOTClient>>*>(val, v_clients));
            } else
                v_clients = it->second;
        }
        break;

    case FOLLOW_TYPE_COMMAND:
        {
            it = m_cmdFollow.find(val);
            if (it == m_cmdFollow.end()) {
                v_clients = new Vector<sp<IOTClient>>();
                m_cmdFollow.insert(
                    std::pair<std::string, Vector<sp<IOTClient>>*>(val, v_clients));
            } else
                v_clients = it->second;
        }
        break;

    default:
        return -1;
    }
    if (!v_clients)
        return -1;

    for (size_t i = 0; i < v_clients->size(); ++i) {
        if ((*v_clients)[i] == target)
            return 0;
    }
    v_clients->add(target);
    return 0;
}

int IOTService::addProperty(const char *key, int type, int size)
{
    return doAddProperty(key, type, size);
}

void IOTService::waitForClient()
{
    ALOGI("IOTService: waitForClient()\n");
    Mutex::Autolock _l(m_lockClis);
    m_condition.wait(m_lockClis);
}

void IOTService::clientActive()
{
    ALOGI("IOTService: clientActive()\n");
    Mutex::Autolock _l(m_lockClis);
    m_condition.signal();
}

IOTService::IOTClient::IOTClient(const char *name, const sp<IOTService> &service)
            : m_name(name), m_service(service), m_command(NULL), m_property(NULL)
{

}

IOTService::IOTClient::~IOTClient()
{
    ALOGI("IOTClient: client [%s] destroy!\n", m_name.c_str());
}

int IOTService::IOTClient::followProperty(String8 &key, int type, int size)
{
    ALOGI("IOTClient: followProperty [%s] type[%d]\n", key.string(), type);
    int ret;
    ret = m_service->addFollowing((int)FOLLOW_TYPE_PROPERTY, key.string(), this);
    if (0 != ret)
        return -1;

    return m_service->addProperty(key.string(), type, size);
}

int IOTService::IOTClient::followCommand(String8 &cmd)
{
    ALOGI("IOTService::IOTClient::followCommand run here\n");
    int ret = m_service->addFollowing((int)FOLLOW_TYPE_COMMAND, cmd.string(), this);
    if (0 != ret)
        return -1;
    return 0;
}

int IOTService::IOTClient::reportEvent(String8 &msg)
{
    ALOGI("IOTService::IOTClient::reportEvent run here\n");
    return m_service->doReportEvent(msg.string(), msg.length());
}

int IOTService::IOTClient::reportProperty(String8 &key, String8 &val)
{
    ALOGI("IOTService::IOTClient::reportProperty run here\n");
    return m_service->doReportProperty(key.string(), val.string());
}

int IOTService::IOTClient::registCommandCB(const sp<IIOTCommandCB> &cmdCB)
{
    ALOGI("IOTService::IOTClient::registCommandCB run here\n");
    if (m_command != NULL)
        return -1;
    m_command = cmdCB;
    return 0;
}

int IOTService::IOTClient::registPropertyCB(const sp<IIOTPropertyCB> &proCB)
{
    ALOGI("IOTService::IOTClient::registPropertyCB run here\n");
    if (m_property != NULL)
        return -1;

    m_property = proCB;
    IInterface::asBinder(m_property)->linkToDeath(this);
    m_service->clientActive();
    return 0;
}

void IOTService::IOTClient::binderDied(const wp<IBinder>& /* who */)
{
    m_service->removeClient(this);
}


} /* end namespace android */
