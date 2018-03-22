#ifndef __IOT_DeviceShadow__H
#define __IOT_DeviceShadow__H

#include "IOT_Service.h"
#include <utils/Singleton.h>
#include <map>
#include <string>

namespace android {

class DeviceShadow : public Singleton<DeviceShadow>, public IOTService {
private:
    friend class Singleton<DeviceShadow>;
    DeviceShadow();

public:
    ~DeviceShadow();

    static inline DeviceShadow& get() { return getInstance(); } 

    void start() { m_thread->run("DeviceShadow"); }
    int doReportEvent(const char* msg, size_t size);
    int doReportProperty(const char *key, const char *val);

    void* m_shadow;
private:
    class IOTThread : public Thread {
    public:
        IOTThread(const sp<DeviceShadow>& service) : m_service(service) {};
        virtual ~IOTThread() {};
        virtual bool threadLoop();
    private:
        sp<DeviceShadow> const m_service;
    };

    bool m_reportFlag;
    sp<IOTThread> m_thread;
};

} /* end namespace android */

#endif
