#include "IIOTPropertyCB.h"
#include <utils/Log.h>

namespace android {

enum {
    IOTACTION_GET_PROPERTY = IBinder::FIRST_CALL_TRANSACTION,
    IOTACTION_SET_PROPERTY,
};

class BpIOTPropertyCB : public BpInterface<IIOTPropertyCB> {
public:
    BpIOTPropertyCB(const sp<IBinder>& impl)
        : BpInterface<IIOTPropertyCB>(impl) { }

    virtual int get(String8 const &key, String8 &val) const {
        Parcel data, reply;
        data.writeInterfaceToken(IIOTPropertyCB::getInterfaceDescriptor());
        data.writeString8(key);
        int status = remote()->transact(IOTACTION_GET_PROPERTY, data, &reply);
        if (status != 0) {
            ALOGW("remote call IOTACTION_GET_PROPERTY error!\n");
            return status;
        }
        val = reply.readString8();
        return reply.readInt32();
    }

    virtual int set(String8 const &key, String8 const &val) const {
        Parcel data, reply;
        data.writeInterfaceToken(IIOTPropertyCB::getInterfaceDescriptor());
        data.writeString8(key);
        data.writeString8(val);
        int status = remote()->transact(IOTACTION_SET_PROPERTY, data, &reply);
        if (status != 0) {
            ALOGW("remote call IOTACTION_SET_PROPERTY error!\n");
            return status;
        }
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(IOTPropertyCB, IOT_PROPERTY_PACK);

status_t BnIOTPropertyCB::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    int ret = -1;
    int len = 0;
    switch (code) {
    case IOTACTION_GET_PROPERTY: {
            CHECK_INTERFACE(IIOTPropertyCB, data, reply);
            String8 key = data.readString8();
            String8 val;
            ret = get(key, val);
            reply->writeString8(val);
            reply->writeInt32(ret);
        }
        break;

    case IOTACTION_SET_PROPERTY: {
            CHECK_INTERFACE(IIOTPropertyCB, data, reply);
            String8 key = data.readString8();
            String8 val = data.readString8();
            ret = set(key, val);
            reply->writeInt32(ret);
        }
        break;

    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
    return 0;
}

}
