LOCAL_PATH := $(call my-dir)

# ============================================================================#
include $(CLEAR_VARS)
LOCAL_MODULE    := libdobby
LOCAL_SRC_FILES := Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)
# ============================================================================#

include $(CLEAR_VARS)

LOCAL_MODULE           := RAMA_MODZ

LOCAL_CFLAGS           := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
LOCAL_CFLAGS           += -fno-rtti -fno-exceptions -fpermissive
LOCAL_CPPFLAGS         := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w -Werror -s -std=c++17 -fexceptions
LOCAL_CPPFLAGS         += -Wno-error=c++11-narrowing -fms-extensions -fno-rtti -fno-exceptions -fpermissive
LOCAL_LDFLAGS          += -Wl,--gc-sections,--strip-all, -llog
LOCAL_ARM_MODE         := arm
LOCAL_LDLIBS           := -llog -landroid -lEGL -lGLESv3 -lGLESv2 -lGLESv1_CM -lz

LOCAL_C_INCLUDES       := $(LOCAL_PATH) \
                          $(LOCAL_PATH)/include \
                          $(LOCAL_PATH)/Dobby \
                          $(LOCAL_PATH)/imgui \
                          $(LOCAL_PATH)/KittyMemory \
                          $(LOCAL_PATH)/Unity \
                          $(LOCAL_PATH)/Struct \
                          $(LOCAL_PATH)/RamaModz/Tools/curl/openssl-android-armeabi-v7a/include

# Explicitly list source files to avoid ODR violations from wildcards
LOCAL_SRC_FILES := Main.cpp \
                   imgui/imgui.cpp \
                   imgui/imgui_draw.cpp \
                   imgui/imgui_widgets.cpp \
                   imgui/imgui_tables.cpp \
                   imgui/imgui_demo.cpp \
                   imgui/imgui_impl_android.cpp \
                   imgui/imgui_impl_opengl3.cpp \
                   xdl/xdl.c \
                   xdl/xdl_iterate.c \
                   xdl/xdl_linker.c \
                   xdl/xdl_lzma.c \
                   xdl/xdl_util.c \
                   KittyMemory/KittyMemory.cpp \
                   KittyMemory/KittyScanner.cpp \
                   KittyMemory/MemoryPatch.cpp \
                   KittyMemory/MemoryBackup.cpp \
                   KittyMemory/KittyUtils.cpp \
                   KittyMemory/KittyArm64.cpp \
                   Unity/Tools.cpp \
                   Unity/MonoString.cpp \
                   Unity/Il2Cpp.cpp

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/And64InlineHook
    LOCAL_SRC_FILES += include/And64InlineHook/And64InlineHook.cpp
else ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Substrate
    # Add substrate sources if needed
endif

LOCAL_STATIC_LIBRARIES := libdobby
LOCAL_CPP_FEATURES     := exceptions

include $(BUILD_SHARED_LIBRARY)
