LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := aewxwidgets
LOCAL_SRC_FILES := dynarray.cpp ffile.cpp hash.cpp hashmap.cpp list.cpp module.cpp object.cpp strconv.cpp \
                   string.cpp file.cpp filefn.cpp filename.cpp tokenzr.cpp


include $(BUILD_STATIC_LIBRARY)


