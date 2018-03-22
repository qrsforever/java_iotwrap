#ifndef __IIOTPropertyCB_H__
#define __IIOTPropertyCB_H__

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

#define IOT_PROPERTY_PACK "com.android.leiot.IIOTPropertyCB"

namespace android {

class IIOTPropertyCB : public IInterface {
public:
    DECLARE_META_INTERFACE(IOTPropertyCB);
    virtual int get(String8 const &key, String8 &val) const = 0;
    virtual int set(String8 const &key, String8 const &val) const = 0;
};

class BnIOTPropertyCB : public BnInterface<IIOTPropertyCB> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel &data,
                                Parcel *reply,
                                uint32_t flags = 0);
};

} /* end namespace android */
#endif
