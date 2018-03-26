#ifndef __IIOT_SERVICE_H__
#define __IIOT_SERVICE_H__

#include "IIOTClient.h"

#define IOT_SERVICE_PACK "com.android.leiot.IIOTService"
#define IOT_SERVICE_NAME "iot_server"

enum {
    DS_STATUS_INVALID = -1,
    DS_STATUS_CONNECTED = 0,
};

namespace android {

class IIOTService : public IInterface {
public:
    DECLARE_META_INTERFACE(IOTService);
    virtual sp<IIOTClient> createClient(String8 name) = 0;
    virtual int getServiceStatus() = 0;
};

class BnIOTService : public BnInterface<IIOTService> {
public:
    virtual status_t onTransact(uint32_t code,
        const Parcel &data,
        Parcel *reply,
        uint32_t flags = 0);
};

} /* end namespace android */

#endif
