#ifndef __IIOTControlCB_H__
#define __IIOTControlCB_H__

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

#define IOT_CONTROL_PACK "com.android.leiot.IIOTControlCB"

namespace android {

class IIOTControlCB : public IInterface {
public:
    DECLARE_META_INTERFACE(IOTControlCB);
    virtual int callback(String8 const &msg) const = 0;
};

class BnIOTControlCB : public BnInterface<IIOTControlCB> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel &data,
                                Parcel *reply,
                                uint32_t flags = 0);
};

} /* end namespace android */
#endif
