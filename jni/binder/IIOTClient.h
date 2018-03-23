#ifndef __IIOTCLIENT_H__
#define __IIOTCLIENT_H__

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

#include "IIOTCommandCB.h"
#include "IIOTPropertyCB.h"

#define IOT_CLIENT_PACK "com.android.leiot.IIOTClient"

#define IOT_PROPERTY_NULL   0
#define IOT_PROPERTY_INT32  1
#define IOT_PROPERTY_STRING 2

/* TODO debug */
#define ALOGI  printf
#define ALOGW  printf
#define ALOGE  printf

namespace android {

class IIOTClient : public IInterface {
public:
    DECLARE_META_INTERFACE(IOTClient);
    virtual int followProperty(String8 &key, int type, int size) = 0;
    virtual int followCommand(String8 &cmd) = 0;
    virtual int reportEvent(String8 &msg) = 0;
    virtual int reportProperty(String8 &key, String8 &val) = 0;
    virtual int registCommandCB(const sp<IIOTCommandCB> &ctlCB) = 0;
    virtual int registPropertyCB(const sp<IIOTPropertyCB> &proCB) = 0;
};

class BnIOTClient : public BnInterface<IIOTClient> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel &data,
                                Parcel *reply,
                                uint32_t flags = 0);
};

} /* end namespace android */
#endif
