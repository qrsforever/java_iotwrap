#ifndef __IOT_DeviceShadow__H
#define __IOT_DeviceShadow__H

#include "IOT_Service.h"
#include <utils/Singleton.h>
#include <map>
#include <string>

extern "C" {
#include "iot_export.h"
#include "iot_import.h"
}

#define ATTR_VALUE_MAX_LEN 256
#define PAYLOAD_MAX_LEN    2048
#define YIELD_TIMEOUT_MS   200
#define COMMAND_MAX_LEN    64

namespace android {

class DeviceShadow : public Singleton<DeviceShadow>, public IOTService {
private:
    friend class Singleton<DeviceShadow>;
    DeviceShadow();

public:
    ~DeviceShadow();

    static inline DeviceShadow& get() { return getInstance(); }

    int doAddProperty(const char *name, int type, int size);
    int doReportEvent(const char* msg, size_t size);
    int doReportProperty(const char *key, const char *val);

    int setupShadow(char *w_buff, char *r_buff);
    int preShadow();
    int postShadow();
    int yieldShadow(int timeout_ms);

    uint32_t getReportPeriod() { return m_reportPeriod; }
    void setReportReriod(uint32_t t) { m_reportPeriod = t; }
    int loadProperties();
    int pushProperties();

    void start() { m_thread->run("DeviceShadow"); }

private:
    class IOTThread : public Thread {
    public:
        IOTThread(const sp<DeviceShadow>& service) : m_service(service) {};
        virtual ~IOTThread() {};
        virtual bool threadLoop();
    private:
        sp<DeviceShadow> const m_service;
    };

    iotx_shadow_attr_pt _addProperty(const char *name, int type, int size, iotx_shadow_attr_cb_t cb);

    class _DS_Attr {
    public:
        _DS_Attr(int flag, iotx_shadow_attr_pt a): report(flag), attr(a) {}
        int report;
        iotx_shadow_attr_pt attr;
    };

    typedef std::map<std::string, struct _DS_Attr>::iterator IterAttrs_t;
    std::map<std::string, struct _DS_Attr> m_iotAttrs;
    bool m_reportFlag;
    uint32_t m_reportPeriod; /* unit: ms */
    void* m_shadow;
    sp<IOTThread> m_thread;
    mutable Mutex m_lockAttrs;
};

} /* end namespace android */

#endif
