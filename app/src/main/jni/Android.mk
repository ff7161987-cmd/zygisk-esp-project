LOCAL_PATH := $(call my-dir)

# ============================================================================#
include $(CLEAR_VARS)
LOCAL_MODULE    := libdobby
LOCAL_SRC_FILES := Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)
# ============================================================================#

include $(CLEAR_VARS)

LOCAL_MODULE           := VM

LOCAL_CFLAGS           := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
LOCAL_CFLAGS           += -fno-rtti -fno-exceptions -fpermissive
LOCAL_CPPFLAGS         := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w -Werror -s -std=c++17
LOCAL_CPPFLAGS         += -Wno-error=c++11-narrowing -fms-extensions -fno-rtti -fno-exceptions -fpermissive
LOCAL_LDFLAGS          += -Wl,--gc-sections,--strip-all -llog
LOCAL_ARM_MODE         := arm
LOCAL_LDLIBS          := -llog -landroid -lEGL -lGLESv3 -lGLESv2 -lGLESv1_CM -lz -ldl

LOCAL_C_INCLUDES       += $(LOCAL_PATH)
LOCAL_C_INCLUDES       += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES       += $(LOCAL_PATH)/Dobby
LOCAL_C_INCLUDES       += $(LOCAL_PATH)/imgui
LOCAL_C_INCLUDES       += $(LOCAL_PATH)/KittyMemory
LOCAL_C_INCLUDES       += $(LOCAL_PATH)/Unity

# Adicionando os novos includes para Hooks e ESP
LOCAL_C_INCLUDES       += $(LOCAL_PATH)/Struct

# Coletando arquivos fonte
FILE_LIST               := $(wildcard $(LOCAL_PATH)/imgui/*.c*)
FILE_LIST               += $(wildcard $(LOCAL_PATH)/xdl/*.c*)
FILE_LIST               += $(wildcard $(LOCAL_PATH)/KittyMemory/*.c*)
FILE_LIST               += $(wildcard $(LOCAL_PATH)/Unity/*.cpp)
FILE_LIST               += $(wildcard $(LOCAL_PATH)/*.cpp)

# Adicionando o novo arquivo ESP.cpp
FILE_LIST               += $(wildcard $(LOCAL_PATH)/ESP.cpp)

LOCAL_SRC_FILES        := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_STATIC_LIBRARIES := libdobby
LOCAL_CPP_FEATURES     := exceptions

include $(BUILD_SHARED_LIBRARY)
