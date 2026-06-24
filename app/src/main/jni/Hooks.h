#ifndef HOOKS_H
#define HOOKS_H

#include <Il2Cpp.h>
#include <android/log.h>

#define LOG_TAG "ZygiskESP"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// Macros para facilitar a busca dinâmica sem offsets
#define GET_METHOD(lib, ns, cls, meth, args) Il2CppGetMethodOffset(OBFUSCATE(lib), OBFUSCATE(ns), OBFUSCATE(cls), OBFUSCATE(meth), args)
#define GET_FIELD(lib, ns, cls, field) Il2CppGetFieldOffset(OBFUSCATE(lib), OBFUSCATE(ns), OBFUSCATE(cls), OBFUSCATE(field))

// Estruturas Unity básicas
struct Vector3 {
    float x, y, z;
    static Vector3 Zero() { return {0, 0, 0}; }
};

struct Vector2 {
    float x, y;
};

// Funções de auto-update para endereços essenciais
namespace UnityFunctions {
    inline void* Camera_get_main = nullptr;
    inline void* Camera_WorldToScreenPoint = nullptr;
    inline void* Transform_get_position = nullptr;
    inline void* Component_get_transform = nullptr;

    inline void UpdateAddresses() {
        Camera_get_main = GET_METHOD("UnityEngine.CoreModule.dll", "UnityEngine", "Camera", "get_main", 0);
        Camera_WorldToScreenPoint = GET_METHOD("UnityEngine.CoreModule.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1);
        Transform_get_position = GET_METHOD("UnityEngine.CoreModule.dll", "UnityEngine", "Transform", "get_position", 0);
        Component_get_transform = GET_METHOD("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_transform", 0);
        
        LOGD("Endereços atualizados via IL2CPP");
    }
}

#endif // HOOKS_H
