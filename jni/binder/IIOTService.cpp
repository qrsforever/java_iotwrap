#include "IIOTService.h"
#include <utils/Log.h>

namespace android {

enum {
    IOTACTION_CREATE_CLIENT = IBinder::FIRST_CALL_TRANSACTION,
};

class BpIOTService : public BpInterface<IIOTService> {
public:
    BpIOTService(const sp<IBinder>& impl)
        : BpInterface<IIOTService>(impl) { }

    virtual sp<IIOTClient> createClient() {
        Parcel data, reply;                                                          
        data.writeInterfaceToken(IIOTService::getInterfaceDescriptor());        
        int status = remote()->transact(IOTACTION_CREATE_CLIENT, data, &reply);      
        if (status != 0) {
            ALOGW("remote call IOTACTION_CREATE_CLIENT error!\n");
            return NULL;
        }
        return interface_cast<IIOTClient>(reply.readStrongBinder());     
    }

};

IMPLEMENT_META_INTERFACE(IOTService, IOT_CLIENT_PACK);

status_t BnIOTService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    int ret = -1;
    switch (code) {
    case IOTACTION_CREATE_CLIENT: {
            CHECK_INTERFACE(IIOTService, data, reply);              
            sp<IBinder> b = IInterface::asBinder(createClient());
            reply->writeStrongBinder(b);
        }
        break;

    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
    return NO_ERROR;
}

}
