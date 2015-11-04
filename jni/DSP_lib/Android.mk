LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libdsp
LOCAL_CFLAGS    := -I. -Wall
#LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/
LOCAL_SRC_FILES := demo.c EQ10.c gain.c Dynamics.c plugin_common.c ReverbStation.c
			  

include $(BUILD_STATIC_LIBRARY)
