#ifndef __IOT_Service_H_
#define __IOT_Service_H_

#include "IIOTService.h"
#include "IIOTControlCB.h"
#include "IIOTPropertyCB.h"

#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <utils/Condition.h> 

#define MAX_CLINETS 10

#define ALOGI  printf
#define ALOGW  printf
#define ALOGE  printf

namespace android {

class IOTService : public BnIOTService, public IBinder::DeathRecipient {
public:
    IOTService ();
    ~IOTService ();

    virtual int doReportEvent(const char *msg, size_t size) = 0;
    virtual int doReportProperty(const char *key, const char *val) = 0;

    int callControlCB(const char *data, size_t size);
    int callPropertyGet(const char *key, char *val, size_t size);
    int callPropertySet(const char *key, const char *val);

    void binderDied(const wp<IBinder>& cli);

protected:
    /* Interfaces of IIOTService */
    sp<IIOTClient> createClient();
    int deleteClient(const sp<IIOTClient> &client);

    /* for parent class */
    void waitForClient();
    void clientActive();
    Condition m_condition;  

private:
    class IOTClient : public BnIOTClient {
    public:
        IOTClient(const sp<IOTService> &service)
            : m_service(service), m_control(NULL), m_property(NULL) {}
        ~IOTClient() {}
    protected:
        /* Interfaces of IIOTClient */
        int reportEvent(String8 &msg);
        int reportProperty(String8 &key, String8 &val);
        int registControlCB(const sp<IIOTControlCB> &ctlCB);
        int registPropertyCB(const sp<IIOTPropertyCB> &proCB);

    private:
        friend class IOTService;
        sp<IOTService> m_service;
        sp<IIOTControlCB> m_control;
        sp<IIOTPropertyCB> m_property;
    };

    Vector< sp<IOTClient> > m_clients;
    Mutex m_lock;
};

} /* end namespace android */

#endif
