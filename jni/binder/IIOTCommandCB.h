#ifndef __IIOTCommandCB_H__
#define __IIOTCommandCB_H__

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

#define IOT_COMMAND_PACK "com.android.leiot.IIOTCommandCB"

namespace android {

class IIOTCommandCB : public IInterface {
public:
    DECLARE_META_INTERFACE(IOTCommandCB);
    virtual int callback(String8 const &cmd, String8 const &msg) const = 0;
};

class BnIOTCommandCB : public BnInterface<IIOTCommandCB> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel &data,
                                Parcel *reply,
                                uint32_t flags = 0);
};

} /* end namespace android */
#endif
