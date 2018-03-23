#include "IIOTCommandCB.h"
#include <utils/Log.h>

namespace android {

enum {
    IOTACTION_COMMAND_CALLBACK = IBinder::FIRST_CALL_TRANSACTION,
};

class BpIOTCommandCB : public BpInterface<IIOTCommandCB> {
public:
    BpIOTCommandCB(const sp<IBinder>& impl)
        : BpInterface<IIOTCommandCB>(impl) { }

    virtual int callback(String8 const &msg) const {
        Parcel data, reply;
        data.writeInterfaceToken(IIOTCommandCB::getInterfaceDescriptor());
        data.writeString8(msg);
        int status = remote()->transact(IOTACTION_COMMAND_CALLBACK, data, &reply);
        if (status != 0) {
            ALOGW("remote call IOTACTION_COMMAND_CALLBACK error!\n");
            return status;
        }
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(IOTCommandCB, IOT_COMMAND_PACK);

status_t BnIOTCommandCB::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    int ret = -1;
    int len = 0;
    switch (code) {
    case IOTACTION_COMMAND_CALLBACK: {
            CHECK_INTERFACE(IIOTCommandCB, data, reply);
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
