# Build iot server
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -DLOG_TAG=\"IOT_Server\"
LOCAL_CFLAGS += -DIOT_DEBUG=1
LOCAL_CFLAGS += -DENABLE_TENCENT_CLOUD
LOCAL_CFLAGS += -DIOT_BUILDTIME=\"$(shell date +%Y%m%d%H%M%S)\"

LOCAL_C_INCLUDES += $(LOCAL_PATH)/binder \
                    $(LOCAL_PATH)/server \
                    $(LOCAL_PATH)/sdk/include

LOCAL_SRC_FILES := server/IOT_Server.cpp \
                   server/IOT_Service.cpp \
                   server/IOT_DeviceShadow.cpp \
                   binder/IIOTService.cpp \
                   binder/IIOTClient.cpp \
                   binder/IIOTCommandCB.cpp \
                   binder/IIOTPropertyCB.cpp


LOCAL_LDFLAGS += -L$(LOCAL_PATH)/sdk/lib -O2
LOCAL_LDFLAGS += \
                 -lleiot_sdk \
                 -lleiot_utils \
                 -lleiot_platform \
                 -lmbedtls \
                 -lMQTTPacketClient \

LOCAL_SHARED_LIBRARIES := \
	libandroid \
	libbinder \
    libutils \
    libcutils \
    liblog

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := iot_server

include $(BUILD_EXECUTABLE)

# build jni client
include $(CLEAR_VARS)

LOCAL_CFLAGS := -DLOG_TAG=\"IOT_Client\"

LOCAL_C_INCLUDES += $(LOCAL_PATH) \
                    $(LOCAL_PATH)/binder \
                    $(LOCAL_PATH)/client

LOCAL_SRC_FILES:= \
	binder/IIOTCommandCB.cpp \
	binder/IIOTPropertyCB.cpp  \
    binder/IIOTService.cpp \
    binder/IIOTClient.cpp \
    client/IOT_CommandCB.cpp \
    client/IOT_PropertyCB.cpp \
    client/IOT_ClientJNI.cpp \
    client/IOT_Helper.cpp \
    IOT_JNI.cpp

LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libnativehelper \
    libbinder \
    libutils \
    libcutils \
    liblog

LOCAL_CFLAGS += -Wall # -Werror -Wno-error=deprecated-declarations -Wunused -Wunreachable-code

LOCAL_MODULE:= libiotclient_jni

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

include $(CLEAR_VARS)                              
LOCAL_MODULE       := testshadow_cert.crt
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/certs
LOCAL_SRC_FILES    := sdk/certs/testshadow_cert.crt
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)                              
LOCAL_MODULE       := testshadow_private.key
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/certs
LOCAL_SRC_FILES    := sdk/certs/testshadow_private.key
include $(BUILD_PREBUILT)

