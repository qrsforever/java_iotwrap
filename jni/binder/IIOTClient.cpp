#include "IIOTClient.h"
#include <utils/Log.h>

namespace android {

enum {
    IOTACTION_FOLLOW_PROPERTY = IBinder::FIRST_CALL_TRANSACTION,
    IOTACTION_FOLLOW_COMMAND,
    IOTACTION_REPORT_EVENT_MESSAGE,
    IOTACTION_REPORT_PROPERTY,
    IOTACTION_REGIST_COMMAND_CALLBACK,
    IOTACTION_REGIST_PROPERTY_CALLBACK,
};

class BpIOTClient : public BpInterface<IIOTClient> {
public:
    BpIOTClient(const sp<IBinder>& impl)
        : BpInterface<IIOTClient>(impl) { }

    virtual int followProperty(String8 &key, int type, int size) {
        Parcel data, reply;
        data.writeInterfaceToken(IIOTClient::getInterfaceDescriptor());
        data.writeString8(key);
        data.writeInt32(type);
        data.writeInt32(size);
        int status = remote()->transact(IOTACTION_FOLLOW_PROPERTY, data, &reply);
        if (status != 0) {
            ALOGW("remote call IOTACTION_REPORT_EVENT error!\n");
            return status;
        }
        return reply.readInt32();
    }

    virtual int followCommand(String8 &cmd) {
        Parcel data, reply;
        data.writeInterfaceToken(IIOTClient::getInterfaceDescriptor());
        data.writeString8(cmd);
        int status = remote()->transact(IOTACTION_FOLLOW_COMMAND, data, &reply);
        if (status != 0) {
            ALOGW("remote call IOTACTION_REPORT_EVENT error!\n");
            return status;
        }
        return reply.readInt32();
    }

    virtual int reportEvent(String8 &msg) {
        Parcel data, reply;
        data.writeInterfaceToken(IIOTClient::getInterfaceDescriptor());
        data.writeString8(msg);
        int status = remote()->transact(IOTACTION_REPORT_EVENT_MESSAGE, data, &reply);
        if (status != 0) {
            ALOGW("remote call IOTACTION_REPORT_EVENT error!\n");
            return status;
        }
        return reply.readInt32();
    }

    virtual int reportProperty(String8 &key, String8 &val) {
        Parcel data, reply;
        data.writeInterfaceToken(IIOTClient::getInterfaceDescriptor());
        data.writeString8(key);
        data.writeString8(val);
        int status = remote()->transact(IOTACTION_REPORT_PROPERTY, data, &reply);
        if (status != 0) {
            ALOGW("remote call IOTACTION_REPORT_EVENT error!\n");
            return status;
        }
        return reply.readInt32();
    }

    virtual int registCommandCB(const sp<IIOTCommandCB>& ctlCB) {
       Parcel data, reply;
       data.writeInterfaceToken(IIOTClient::getInterfaceDescriptor());
       data.writeStrongBinder(IInterface::asBinder(ctlCB));
       int status = remote()->transact(IOTACTION_REGIST_COMMAND_CALLBACK, data, &reply);
       if (status != 0) {
           ALOGW("remote call IOTACTION_REGIST_COMMAND_CALLBACK error!\n");
           return status;
       }
       return reply.readInt32();
    }

    virtual int registPropertyCB(const sp<IIOTPropertyCB>& proCB) {
       Parcel data, reply;
       data.writeInterfaceToken(IIOTClient::getInterfaceDescriptor());
       data.writeStrongBinder(IInterface::asBinder(proCB));
       int status = remote()->transact(IOTACTION_REGIST_PROPERTY_CALLBACK, data, &reply);
       if (status != 0) {
           ALOGW("remote call IOTACTION_REGIST_PROPERTY_CALLBACK error!\n");
           return status;
       }
       return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(IOTClient, IOT_CLIENT_PACK);

status_t BnIOTClient::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    int ret = -1;
    switch (code) {
    case IOTACTION_FOLLOW_PROPERTY: {
            CHECK_INTERFACE(IIOTClient, data, reply);
            String8 key = data.readString8();
            ret = followProperty(key, data.readInt32(), data.readInt32());
            reply->writeInt32(ret);
        }
        break;

    case IOTACTION_FOLLOW_COMMAND: {
            CHECK_INTERFACE(IIOTClient, data, reply);
            String8 cmd = data.readString8();
            ret = followCommand(cmd);
            reply->writeInt32(ret);
        }
        break;

    case IOTACTION_REPORT_EVENT_MESSAGE: {
            CHECK_INTERFACE(IIOTClient, data, reply);
            String8 msg = data.readString8();
            ret = reportEvent(msg);
            reply->writeInt32(ret);
        }
        break;

    case IOTACTION_REPORT_PROPERTY: {
            CHECK_INTERFACE(IIOTClient, data, reply);
            String8 key = data.readString8();
            String8 val = data.readString8();
            ret = reportProperty(key, val);
            reply->writeInt32(ret);
        }
        break;

    case IOTACTION_REGIST_COMMAND_CALLBACK: {
            CHECK_INTERFACE(IIOTClient, data, reply);
            sp<IIOTCommandCB> ctlCB = interface_cast<IIOTCommandCB>(data.readStrongBinder());
            ret = registCommandCB(ctlCB);
            reply->writeInt32(ret);
        }
        break;

    case IOTACTION_REGIST_PROPERTY_CALLBACK: {
            CHECK_INTERFACE(IIOTClient, data, reply);
            sp<IIOTPropertyCB> proCB = interface_cast<IIOTPropertyCB>(data.readStrongBinder());
            ret = registPropertyCB(proCB);
            reply->writeInt32(ret);
        }
        break;

    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
    return NO_ERROR;
}

} /* end namespace android */
