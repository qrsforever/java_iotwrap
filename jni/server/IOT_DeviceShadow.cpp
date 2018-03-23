#include "IOT_DeviceShadow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#define PRODUCT_KEY                 "38GBCH2FO2"
#define DEVICE_NAME                 "testshadow"
#define DEVICE_SECRET               "123456"
static char sg_cert_file[128];
static char sg_key_file[128];

#define SHADOW_MQTT_MSGLEN      (2048)

#define SHADOW_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

extern "C" {

static void _DS_call_default_property_cb(iotx_shadow_attr_pt pattr)
{
    SHADOW_TRACE(" _DS_call_default_property_cb (%s)", pattr->pattr_name);

    char value[ATTR_VALUE_MAX_LEN];
    switch(pattr->attr_type) {
    case IOTX_SHADOW_INT32:
        HAL_Snprintf(value, ATTR_VALUE_MAX_LEN, "%d", *(int32_t*)pattr->pattr_data);
        break;

    case IOTX_SHADOW_STRING:
        HAL_Snprintf(value, ATTR_VALUE_MAX_LEN, "%s", (char*)pattr->pattr_data);
        break;
    default:
        return;
    }

    android::DeviceShadow::get().callPropertySet(pattr->pattr_name, value);
}

static void _DS_call_default_command_cb(void* handle, const char *data, size_t data_len)
{
    SHADOW_TRACE(" _DS_call_default_command_cb (%s)", data);
    /* TODO not impl */
    android::DeviceShadow::get().callCommandCB("video_search", data);
}

}

namespace android {

ANDROID_SINGLETON_STATIC_INSTANCE(DeviceShadow);


DeviceShadow::DeviceShadow()
    : m_shadow(0), m_thread(0)
{/*{{{*/
    m_thread = new IOTThread(this);

    // static add
    // _addProperty("power",      IOTX_SHADOW_INT32, sizeof(int32_t), _DS_call_default_property_cb);
    // _addProperty("brightness", IOTX_SHADOW_INT32, sizeof(int32_t), _DS_call_default_property_cb);
    // _addProperty("signal",     IOTX_SHADOW_INT32, sizeof(int32_t), _DS_call_default_property_cb);

}/*}}}*/

DeviceShadow::~DeviceShadow()
{/*{{{*/
    m_thread.clear();

    /* TODO release m_iotAttrs */
}/*}}}*/

int DeviceShadow::doAddProperty(const char *name, int type, int size)
{/*{{{*/
    if (!name || size <= 0)
        return -1;

    int iot_type;
    switch (type) {
    case IOT_PROPERTY_INT32:
        iot_type = IOTX_SHADOW_INT32;
        break;
    case IOT_PROPERTY_STRING:
        iot_type = IOTX_SHADOW_STRING;
        break;
    default:
        return -1;
    }

    return _addProperty(name, iot_type, size, _DS_call_default_property_cb);
}/*}}}*/

int DeviceShadow::doReportEvent(const char* msg, size_t size)
{/*{{{*/
    if (!msg || size == 0)
        return -1;
    return IOT_Shadow_Event_Report(m_shadow, (char*)msg, size);
}/*}}}*/

int DeviceShadow::doReportProperty(const char *key, const char *val)
{/*{{{*/
    if (!key || !val)
        return -1;
    Mutex::Autolock _l(m_lockAttrs);

    IterAttrs_t it = m_iotAttrs.find(key);
    if (it == m_iotAttrs.end())
        return -1;

    it->second.report = 1;
    iotx_shadow_attr_pt attr = it->second.attr;
    switch(attr->attr_type) {
    case IOTX_SHADOW_INT32:
        *(int32_t*)attr->pattr_data = atoi(val);
        break;

    case IOTX_SHADOW_STRING:
        strncpy((char*)attr->pattr_data, val, strlen(val));
        break;
    default:
        ;
    }
    return 0;
}/*}}}*/

int DeviceShadow::_addProperty(const char *name, int type, int size, iotx_shadow_attr_cb_t cb)
{/*{{{*/
    if (!name || size <= 0)
        return -1;
    Mutex::Autolock _l(m_lockAttrs);

    IterAttrs_t it = m_iotAttrs.find(name);
    if (it != m_iotAttrs.end()) {
        HAL_Printf("already name[%s] type[%d vs %d] size[%d]\n", name, type, it->second.attr->attr_type, size);
        if (type != it->second.attr->attr_type)
            return -1;
        return 0;
    }

    iotx_shadow_attr_pt attr = (iotx_shadow_attr_pt)calloc(1, sizeof(iotx_shadow_attr_t));
    if (!attr)
        return -1;

    char *pname = (char*)calloc(strlen(name) + 1, 1);
    if (!pname) {
        free(attr);
        return -1;
    }
    strcpy(pname, name);

    attr->pattr_name = pname;
    attr->pattr_data = calloc(1, size);
    attr->attr_type = (iotx_shadow_attr_datatype_t)type;
    attr->mode = cb ? IOTX_SHADOW_RW : IOTX_SHADOW_READONLY;
    attr->callback = cb;

    HAL_Printf("addProperty(%s %d %d)\n", name, type, size);
    m_iotAttrs.insert(std::pair<std::string, _DS_Attr>(name, _DS_Attr(1, attr)));
    return 0;
}/*}}}*/

int DeviceShadow::loadProperties()
{/*{{{*/
    Mutex::Autolock _l(m_lockAttrs);

    int ret = -1;
    IterAttrs_t it;
    for (it = m_iotAttrs.begin(); it != m_iotAttrs.end(); ++it) {
        _DS_Attr &ds_attr = it->second;
        iotx_shadow_attr_pt attr = ds_attr.attr;
        if (attr) {
            switch(attr->attr_type) {
            case IOTX_SHADOW_INT32:
                {
                    char value[32] = { 0 };
                    ret = callPropertyGet(attr->pattr_name, value, 32);
                    if (ret > 0) {
                        int32_t told = *(int32_t*)attr->pattr_data;
                        int32_t tnew = atoi(value);
                        if (told != tnew) {
                            ds_attr.report = 1;
                            *(int32_t*)attr->pattr_data = tnew;
                        }
                    }
                }
                break;

            case IOTX_SHADOW_STRING:
                {
                    char value[ATTR_VALUE_MAX_LEN] = { 0 };
                    ret = callPropertyGet(attr->pattr_name, value, ATTR_VALUE_MAX_LEN);
                    if (ret > 0) {
                        /* TODO compare strlen length */
                        char *told = (char*)attr->pattr_data;
                        char *tnew = value;
                        if (strcmp(told, tnew)) {
                            ds_attr.report = 1;
                            strncpy((char*)attr->pattr_data, tnew, ret);
                        }
                    }
                }
                break;
            default:
                continue;
            }
        }
    }
    return 0;
}/*}}}*/

int DeviceShadow::pushProperties()
{/*{{{*/
    Mutex::Autolock _l(m_lockAttrs);

    int ret = 0, flg = 0;
    static char s_buff[PAYLOAD_MAX_LEN] = { 0 };
    format_data_t format;                                              

    IterAttrs_t it;
    for (it = m_iotAttrs.begin(); it != m_iotAttrs.end(); ++it) {
        _DS_Attr &ds_attr = it->second;
        if (ds_attr.report) {
            if (0 == flg) {
                 IOT_Shadow_PushFormat_Init(m_shadow, &format, s_buff, PAYLOAD_MAX_LEN);
                 flg = 1;
            }
            IOT_Shadow_PushFormat_Add(m_shadow, &format, ds_attr.attr);   
            ds_attr.report = 0;
        }
    }
    if (1 == flg) {
        IOT_Shadow_PushFormat_Finalize(m_shadow, &format);                 
        ret = IOT_Shadow_Push(m_shadow, format.buf, format.offset, YIELD_TIMEOUT_MS);    
    }
    return ret;
}/*}}}*/

int DeviceShadow::setupShadow(char *w_buff, char *r_buff)
{/*{{{*/
    int rc;
    iotx_conn_info_pt puser_info;
    void *h_shadow;
    iotx_shadow_para_t shadow_para;

    rc = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&puser_info);
    if (SUCCESS_RETURN != rc) {
        SHADOW_TRACE("rc = IOT_SetupConnInfo() = %d", rc);
        return rc;
    }

    memset(&shadow_para, 0, sizeof(iotx_shadow_para_t));

    sprintf(sg_cert_file, "/data/certs/%s_cert.crt", DEVICE_NAME);
    sprintf(sg_key_file, "/data/certs/%s_private.key", DEVICE_NAME);
    shadow_para.mqtt.cert_file = sg_cert_file;
    shadow_para.mqtt.key_file = sg_key_file;

    shadow_para.mqtt.port = puser_info->port;
    shadow_para.mqtt.host = puser_info->host_name;
    shadow_para.mqtt.client_id = puser_info->client_id;
    shadow_para.mqtt.username = puser_info->username;
    shadow_para.mqtt.password = puser_info->password;
    shadow_para.mqtt.pub_key = puser_info->pub_key;

    shadow_para.mqtt.request_timeout_ms = 2000;
    shadow_para.mqtt.clean_session = 0;
    shadow_para.mqtt.keepalive_interval_ms = 60000;
    shadow_para.mqtt.pread_buf = r_buff;
    shadow_para.mqtt.read_buf_size = SHADOW_MQTT_MSGLEN;
    shadow_para.mqtt.pwrite_buf = w_buff;
    shadow_para.mqtt.write_buf_size = SHADOW_MQTT_MSGLEN;

    shadow_para.mqtt.handle_event.h_fp = NULL;
    shadow_para.mqtt.handle_event.pcontext = NULL;

    h_shadow = IOT_Shadow_Construct(&shadow_para);
    if (NULL == h_shadow) {
        SHADOW_TRACE("construct device shadow failed!");
        return rc;
    }
    m_shadow = h_shadow;
    return 0;
}/*}}}*/

int DeviceShadow::preShadow()
{/*{{{*/
    /* 1. wait the first client create */
    waitForClient();

    /* 2. register attrs to iotsdk */
    IterAttrs_t it;
    for (it = m_iotAttrs.begin(); it != m_iotAttrs.end(); ++it) {
        _DS_Attr &ds_attr = it->second;
        if (ds_attr.attr) {
            IOT_Shadow_RegisterAttribute(m_shadow, ds_attr.attr);
            SHADOW_TRACE("Register attr[%s]", ds_attr.attr->pattr_name);
        }
    }

    /* 3. register control to iotsdk */
    IOT_Shadow_Control_Register(m_shadow, _DS_call_default_command_cb);

    /* 4. sync attrs from iotcloud */
    IOT_Shadow_Pull(m_shadow);
    return 0;
}/*}}}*/

int DeviceShadow::postShadow()
{/*{{{*/
    /* 1. delete attrs from qcloud */
    IterAttrs_t it;
    for (it = m_iotAttrs.begin(); it != m_iotAttrs.end(); ++it) {
        _DS_Attr &ds_attr = it->second;
        if (ds_attr.attr) {
            SHADOW_TRACE("Delete attr[%s]", ds_attr.attr->pattr_name);
            IOT_Shadow_DeleteAttribute(m_shadow, ds_attr.attr);
        }
    }

    IOT_Shadow_Destroy(m_shadow);
    m_shadow = 0;
    return 0;
}/*}}}*/

int DeviceShadow::yieldShadow(int timeout_ms)
{/*{{{*/
    IOT_Shadow_Yield(m_shadow, timeout_ms);
    return 0;
}/*}}}*/

bool DeviceShadow::IOTThread::threadLoop()
{/*{{{*/
    IOT_OpenLog("shadow");
    IOT_SetLogLevel(IOT_LOG_DEBUG);
    int ret;

    const uint32_t up_timeout_ms = 300000;
    uint64_t empired_ms = 0;
    char *w_buff = (char *)HAL_Malloc(SHADOW_MQTT_MSGLEN);
    char *r_buff = (char *)HAL_Malloc(SHADOW_MQTT_MSGLEN);

    ret = m_service->setupShadow(w_buff, r_buff);
    if (ret < 0) {
        SHADOW_TRACE("error!");
        goto END;
    }
    m_service->preShadow();

    empired_ms = HAL_UptimeMs() + up_timeout_ms;
    do {

        m_service->yieldShadow(YIELD_TIMEOUT_MS);
        if (HAL_UptimeMs() > empired_ms) {
            m_service->loadProperties();
            empired_ms = HAL_UptimeMs() + up_timeout_ms;
        }
        m_service->pushProperties();

    } while (isRunning());

    /* TODO never run here ! */
    m_service->postShadow();

END:
    HAL_Free(w_buff);
    HAL_Free(r_buff);

    SHADOW_TRACE("out of demo!");
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();
    return false;
}/*}}}*/

} /* end namespace android */
