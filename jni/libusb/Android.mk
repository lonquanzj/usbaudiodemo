LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libesdusb
LOCAL_CFLAGS    := -I. -Wall
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_SRC_FILES := descriptor.c \
                   sync.c \
                   core.c \
                   hotplug.c \
                   io.c \
                   strerror.c \
                   os/linux_usbfs.c \
                   os/threads_posix.c \
                   os/poll_posix.c \
                   os/linux_netlink.c
				   

include $(BUILD_SHARED_LIBRARY)

