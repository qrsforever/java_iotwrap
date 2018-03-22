#include "IIOTControlCB.h"
#include <utils/Log.h>

namespace android {

enum {
    IOTACTION_CONTROL_CALLBACK = IBinder::FIRST_CALL_TRANSACTION,
};

class BpIOTControlCB : public BpInterface<IIOTControlCB> {
public:
    BpIOTControlCB(const sp<IBinder>& impl)
        : BpInterface<IIOTControlCB>(impl) { }

    virtual int callback(String8 const &msg) const {
        Parcel data, reply;
        data.writeInterfaceToken(IIOTControlCB::getInterfaceDescriptor());
        data.writeString8(msg);
        int status = remote()->transact(IOTACTION_CONTROL_CALLBACK, data, &reply);
        if (status != 0) {
            ALOGW("remote call IOTACTION_CONTROL_CALLBACK error!\n");
            return status;
        }
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(IOTControlCB, IOT_CONTROL_PACK);

status_t BnIOTControlCB::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    int ret = -1;
    int len = 0;
    switch (code) {
    case IOTACTION_CONTROL_CALLBACK: {
            CHECK_INTERFACE(IIOTControlCB, data, reply);
            String8 msg = data.readString8();
            ret = callback(msg);
            reply->writeInt32(ret);
        }
        break;

    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
    return 0;
}

}
