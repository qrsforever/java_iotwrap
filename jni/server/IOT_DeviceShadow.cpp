#include "IOT_DeviceShadow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

extern "C" {
#include "iot_export.h"
#include "iot_import.h"
}

#define PRODUCT_KEY                 "38GBCH2FO2"
#define DEVICE_NAME                 "testshadow"
#define DEVICE_SECRET               "123456"
static char sg_cert_file[128];
static char sg_key_file[128];

#define SHADOW_MQTT_MSGLEN      (1024)

#define SHADOW_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

extern  "C" {/*{{{*/
static void _device_shadow_cb_light(iotx_shadow_attr_pt pattr)
{
    SHADOW_TRACE("----");
    SHADOW_TRACE("Attrbute Name: '%s'", pattr->pattr_name);
    SHADOW_TRACE("Attrbute Value: %d", *(int32_t *)pattr->pattr_data);
    SHADOW_TRACE("----");
    // DeviceShadow::self().callPropertySet("light", "1");
}

static void _device_shadow_control_cb(void* handle, const char *data, size_t data_len)
{
    SHADOW_TRACE("data[%s]", data);
    // DeviceShadow::self().callControlCB(data, data_len);
}

} /* end extern "C" *//*}}}*/

/*
    attr_light.attr_type = IOTX_SHADOW_INT32;
    attr_light.mode = IOTX_SHADOW_RW;
    attr_light.pattr_name = "switch";
    attr_light.pattr_data = &light;
    attr_light.callback = _device_shadow_cb_light;

static iotx_shadow_attr_t[] {
    {.pattr_name = "signalsource", IOTX_SHADOW_RW, "},
}
*/
static int demo_device_shadow(char *msg_buf, char *msg_readbuf)
{
    char buf[1024];
    int rc;
    iotx_conn_info_pt puser_info;
    void *h_shadow;
    iotx_shadow_para_t shadow_para;

    /* Device AUTH */
    rc = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&puser_info);
    if (SUCCESS_RETURN != rc) {
        SHADOW_TRACE("rc = IOT_SetupConnInfo() = %d", rc);
        return rc;
    }

    /* Construct a device shadow */
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
    shadow_para.mqtt.pread_buf = msg_readbuf;
    shadow_para.mqtt.read_buf_size = SHADOW_MQTT_MSGLEN;
    shadow_para.mqtt.pwrite_buf = msg_buf;
    shadow_para.mqtt.write_buf_size = SHADOW_MQTT_MSGLEN;

    shadow_para.mqtt.handle_event.h_fp = NULL;
    shadow_para.mqtt.handle_event.pcontext = NULL;

    h_shadow = IOT_Shadow_Construct(&shadow_para);
    if (NULL == h_shadow) {
        SHADOW_TRACE("construct device shadow failed!");
        return rc;
    }

    android::DeviceShadow::get().m_shadow = h_shadow;

    /* Define and add two attribute */

    int32_t light = 1000, temperature = 1001;
    iotx_shadow_attr_t attr_light, attr_temperature;

    memset(&attr_light, 0, sizeof(iotx_shadow_attr_t));
    memset(&attr_temperature, 0, sizeof(iotx_shadow_attr_t));

    /* Initialize the @light attribute */
    attr_light.attr_type = IOTX_SHADOW_INT32;
    attr_light.mode = IOTX_SHADOW_RW;
    attr_light.pattr_name = "switch";
    attr_light.pattr_data = &light;
    attr_light.callback = _device_shadow_cb_light;

    /* Initialize the @temperature attribute */
    attr_temperature.attr_type = IOTX_SHADOW_INT32;
    attr_temperature.mode = IOTX_SHADOW_READONLY;
    attr_temperature.pattr_name = "temperature";
    attr_temperature.pattr_data = &temperature;
    attr_temperature.callback = NULL;

    /* Register the attribute */
    /* Note that you must register the attribute you want to synchronize with cloud
     * before calling IOT_Shadow_Pull() */
    IOT_Shadow_RegisterAttribute(h_shadow, &attr_light);
    IOT_Shadow_RegisterAttribute(h_shadow, &attr_temperature);

    IOT_Shadow_Control_Register(h_shadow, _device_shadow_control_cb);

    /* synchronize the device shadow with device shadow cloud */
    IOT_Shadow_Pull(h_shadow);

    int i = 0;
    int ret = 0;
    int report_flag = 1;
    do {
        if (report_flag || (i % 11) == 0) {
            format_data_t format;
            /* Format the attribute data */
            IOT_Shadow_PushFormat_Init(h_shadow, &format, buf, 1024);
            IOT_Shadow_PushFormat_Add(h_shadow, &format, &attr_temperature);
            IOT_Shadow_PushFormat_Add(h_shadow, &format, &attr_light);
            IOT_Shadow_PushFormat_Finalize(h_shadow, &format);

            /* Update attribute data */
            ret = IOT_Shadow_Push(h_shadow, format.buf, format.offset, 10);
            if (ret == SUCCESS_RETURN) {
                report_flag = 0;
            }
        }
        i++;

        IOT_Shadow_Yield(h_shadow, 200);

    } while (1);

    IOT_Shadow_DeleteAttribute(h_shadow, &attr_temperature);
    IOT_Shadow_DeleteAttribute(h_shadow, &attr_light);

    IOT_Shadow_Destroy(h_shadow);

    return 0;
}

namespace android {

ANDROID_SINGLETON_STATIC_INSTANCE(DeviceShadow);


struct _DS_Attributes {
    int signalSource;

    int volumn;
    char bgColor[16];
} g_Attrs;

static std::map<std::string, iotx_shadow_attr_pt> m_iotAttrs;

DeviceShadow::DeviceShadow()
    : m_shadow(0), m_thread(0)
    , m_reportFlag(false)
{
    m_thread = new IOTThread(this);

    // m_iotAttrs.insert(std::pair<std::string, 
}

DeviceShadow::~DeviceShadow()
{
    m_thread.clear();
}

int DeviceShadow::doReportEvent(const char* msg, size_t size)
{
    return 0;
}

int DeviceShadow::doReportProperty(const char *key, const char *val)
{
    m_reportFlag = true;
    return 0;
}

bool DeviceShadow::IOTThread::threadLoop()
{
    IOT_OpenLog("shadow");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    char *msg_buf = (char *)HAL_Malloc(SHADOW_MQTT_MSGLEN);
    char *msg_readbuf = (char *)HAL_Malloc(SHADOW_MQTT_MSGLEN);

    demo_device_shadow(msg_buf, msg_readbuf);

    HAL_Free(msg_buf);
    HAL_Free(msg_readbuf);

    SHADOW_TRACE("out of demo!");
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();
    return false;
}

} /* end namespace android */
