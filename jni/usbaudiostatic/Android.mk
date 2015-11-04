LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := usbaudiostatic
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libaeusbaudio.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)