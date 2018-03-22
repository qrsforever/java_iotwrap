LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

src_dirs : src

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, $(src_dirs)) 

LOCAL_CERTIFICATE := platform

LOCAL_DEX_PREOPT := false

LOCAL_MODULE := leiot

include $(BUILD_JAVA_LIBRARY)

# Use the folloing include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))
