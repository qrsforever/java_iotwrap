LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

src_dirs : src

LOCAL_SRC_FILES := $(call all-java-files-under, $(src_dirs))

LOCAL_CERTIFICATE := platform
LOCAL_MODULE := leiot
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_NO_EMMA_INSTRUMENT := true
LOCAL_NO_EMMA_COMPILE := true
LOCAL_DX_FLAGS := --core-library
include $(BUILD_JAVA_LIBRARY)

# Use the folloing include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))
