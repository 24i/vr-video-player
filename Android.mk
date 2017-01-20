LOCAL_PATH := $(call my-dir)

APP_ABI := armeabi-v7a

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
include $(CLEAR_VARS)
LOCAL_MODULE:= libavcodec
LOCAL_SRC_FILES:= lib/libavcodec.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libavformat
LOCAL_SRC_FILES:= lib/libavformat.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libswscale
LOCAL_SRC_FILES:= lib/libswscale.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libavutil
LOCAL_SRC_FILES:= lib/libavutil.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libwsresample
LOCAL_SRC_FILES:= lib/libswresample.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
TARGET_OUT="bin/Assets/Plugins/Android/$(TARGET_ARCH_ABI)"
LOCAL_MODULE := main
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/src/plugin/**/*.c)
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += -UNDEBUG
LOCAL_CFLAGS += -DUNITY_ANDROID
LOCAL_STATIC_LIBRARIES := libavformat libavcodec libswscale libswresample
include $(BUILD_SHARED_LIBRARY)
endif