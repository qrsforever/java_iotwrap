#include "IOT_DeviceShadow.h"
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>

using namespace android;

int main(int , char**)
{
#if defined(IOT_BUILDTIME)
    ALOGI("main iot_sever start, pid = %ld, buildtime = %s\n", getpid(), IOT_BUILDTIME);
#endif

    sp<IServiceManager> sm = defaultServiceManager();
    sm->addService(String16(IOT_SERVICE_NAME), &DeviceShadow::get());
    sp<ProcessState> proc(ProcessState::self());

    DeviceShadow::get().start();

    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    ALOGE("main iot_server quit\n");
    return 0;
}
