LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := usbtestnative
LOCAL_SRC_FILES := \
    AndroidDebugLogger.cpp \
    InputMonitorBuffer.cpp \
    LogFileLogger.cpp \
    MailBox.cpp \
    ToJava.cpp \
    USBControl.cpp \
    WavFile.cpp \
    OpenSLStream.cpp \
    test.cpp \
    myaes.cpp	\
    HIDThansfer.cpp \
    DataTransform.cpp \
    wrap/USBTestNative_wrap.cpp

LOCAL_C_INCLUDES :=  $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/../wxWidgets

LOCAL_STATIC_LIBRARIES := aewxwidgets usbaudiostatic dsp
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog -lz -lOpenSLES
LOCAL_SHARED_LIBRARIES := esdusb

include $(BUILD_SHARED_LIBRARY)


