#ifndef __IOT_Service_H_
#define __IOT_Service_H_

#include "IIOTService.h"
#include "IIOTCommandCB.h"
#include "IIOTPropertyCB.h"

#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <utils/Condition.h> 
#include <string>
#include <map>

#define MAX_CLINETS 10

#define ALOGI  printf
#define ALOGW  printf
#define ALOGE  printf

enum {
    FOLLOW_TYPE_PROPERTY = 0,
    FOLLOW_TYPE_COMMAND,
};

namespace android {

class IOTService : public BnIOTService {
public:
    IOTService ();
    ~IOTService ();

    /* TODO support add property dynamicly */
    virtual int doAddProperty(const char *name, int type, int size) {return -1;}
    virtual int doReportEvent(const char *msg, size_t size) = 0;
    virtual int doReportProperty(const char *key, const char *val) = 0;

    int callCommandCB(const char *cmd, const char *data);
    int callPropertyGet(const char *key, char *val, size_t size);
    int callPropertySet(const char *key, const char *val);

protected:
    /* Interfaces of IIOTService */
    sp<IIOTClient> createClient(String8 name);

    int removeClient(const sp<IIOTClient> &client);
    /* for parent class */
    void waitForClient();
    void clientActive();
    Condition m_condition;  

private:
    class IOTClient : public BnIOTClient, public IBinder::DeathRecipient {
    public:
        IOTClient(const char *name, const sp<IOTService> &service);
        ~IOTClient();

    protected:
        /* Interfaces of IIOTClient */
        int followProperty(String8 &key, int type, int size);
        int followCommand(String8 &cmd);
        int reportEvent(String8 &msg);
        int reportProperty(String8 &key, String8 &val);
        int registCommandCB(const sp<IIOTCommandCB> &cmdCB);
        int registPropertyCB(const sp<IIOTPropertyCB> &proCB);

        void binderDied(const wp<IBinder>& /* who */);

    private:
        friend class IOTService;
        const std::string m_name;
        sp<IOTService> m_service;
        sp<IIOTCommandCB> m_command;
        sp<IIOTPropertyCB> m_property;
    };

    int addFollowing(int type, const char *val, const sp<IOTClient> &target);
    int addProperty(const char *key, int type, int size);

    Mutex m_lockClis;
    Mutex m_lockFollow;
    Vector< sp<IOTClient> > m_clients;
    typedef std::map<std::string, Vector<sp<IOTClient>>* >::iterator IterFollow;
    std::map<std::string, Vector<sp<IOTClient>>*> m_keyFollow;
    std::map<std::string, Vector<sp<IOTClient>>*> m_cmdFollow;
};

} /* end namespace android */

#endif
