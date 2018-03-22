#ifndef __IIOT_SERVICE_H__
#define __IIOT_SERVICE_H__

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

#include "IIOTClient.h"

#define IOT_SERVICE_PACK "com.android.leiot.IIOTService"
#define IOT_SERVICE_NAME "iot_server"

namespace android {

class IIOTService : public IInterface {
public:
    DECLARE_META_INTERFACE(IOTService);
    virtual sp<IIOTClient> createClient() = 0;
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
