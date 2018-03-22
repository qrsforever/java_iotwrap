#ifndef __IIOTCLIENT_H__
#define __IIOTCLIENT_H__

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

#include "IIOTControlCB.h"
#include "IIOTPropertyCB.h"

#define IOT_CLIENT_PACK "com.android.leiot.IIOTClient"

namespace android {

class IIOTClient : public IInterface {
public:
    DECLARE_META_INTERFACE(IOTClient);
    virtual int reportEvent(String8 &msg) = 0;
    virtual int reportProperty(String8 &key, String8 &val) = 0;
    virtual int registControlCB(const sp<IIOTControlCB> &ctlCB) = 0;
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
