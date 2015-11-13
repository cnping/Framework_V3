LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	main_mediaserver.cpp 

LOCAL_SHARED_LIBRARIES := \
	libaudioflinger \
	libcameraservice \
	libdisplayservice \
	libmediaplayerservice \
	libutils \
	libbinder \
#libcedarxosal \

# FIXME The duplicate audioflinger is temporary
LOCAL_C_INCLUDES := \
    frameworks/av/media/libmediaplayerservice \
    frameworks/av/services/audioflinger \
    frameworks/av/services/camera/libcameraservice \
    frameworks/av/services/display/libdisplayservice \
    frameworks/native/services/audioflinger \
    frameworks/av/media/CedarX-Projects/CedarVideoLib/cedarv_osal \
    device/softwinner/common/hardware/include 

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= mediaserver

include $(BUILD_EXECUTABLE)
