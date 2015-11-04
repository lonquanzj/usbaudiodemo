JNIPATH := $(call my-dir)
LOCAL_PATH := $(JNIPATH)


include $(call all-subdir-makefiles)
TARGET_PLATFORM := android-12
LOCAL_PATH := $(JNIPATH)
include $(CLEAR_VARS)


#include $(BUILD_SHARED_LIBRARY)


