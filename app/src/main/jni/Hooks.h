#ifndef HOOKS_H
#define HOOKS_H

#include <Il2Cpp.h>
#include <android/log.h>
#include "Struct/Class.h" // Inclui as definições de Vvector3, monoString e macros existentes
#include "Unity/Quaternion.h" // Inclui Quaternion
#include <dlfcn.h> // Para dlsym

#define LOG_TAG "ZygiskESP"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// Definição de DobbySymbolResolver usando dlsym
// Esta função não é parte do Dobby padrão, mas é comumente usada em projetos Zygisk
// para resolver símbolos dinamicamente. Se já existir em xdl.h ou outra lib,
// esta definição será redundante, mas evita o erro de 'não encontrado'.
inline void* DobbySymbolResolver(const char* libraryName, const char* symbolName) {
    void* handle = dlopen(libraryName, RTLD_LAZY);
    if (handle) {
        void* symbol = dlsym(handle, symbolName);
        if (symbol) {
            return symbol;
        } else {
            LOGD("DobbySymbolResolver: Símbolo %s não encontrado em %s", symbolName, libraryName);
        }
        dlclose(handle);
    } else {
        LOGD("DobbySymbolResolver: Biblioteca %s não pode ser aberta", libraryName);
    }
    return nullptr;
}

// Funções de auto-update para endereços essenciais da Unity
namespace UnityFunctions {
    inline void UpdateAddresses() {
        LOGD("UnityFunctions: Usando macros e wrappers de Class.h para endereços Unity.");
    }

    inline void* GetMainCamera() {
        return Camera_main();
    }

    inline Vvector3 WorldToScreenPoint(void* camera, Vvector3 worldPos) {
        return ::WorldToScreenPoint(camera, worldPos);
    }

    inline void* GetTransform(void* obj) {
        return Component_GetTransform(obj);
    }

    inline Vvector3 GetPosition(void* transform) {
        return get_position(transform);
    }

    inline const char* GetPlayerName(void* player) {
        monoString* nameStr = get_NickName(player);
        if (nameStr) return nameStr->getChars();
        return "Unknown";
    }

    inline bool IsLocalPlayer(void* player) {
        return IsLocal(player);
    }

    inline bool IsEnemy(void* player) {
        return !IsLocal(player) && !get_isLocalTeam(player);
    }

    inline Vvector3 GetPlayerPosition(void* player) {
        return get_position(GetTransform(player));
    }

    inline bool IsVisible(void* player) {
        return get_isVisible(player);
    }

    inline bool IsDieing(void* player) {
        return get_IsDieing(player);
    }

    inline int GetHP(void* player) {
        return GetHp(player);
    }
}

#endif // HOOKS_H
