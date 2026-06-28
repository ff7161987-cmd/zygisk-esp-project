#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <Il2Cpp.h>
#include <Vector3.hpp>
#include <android/log.h>
#include "obfuscate.h"

// Configurações de Log
#define ULOG_TAG OBFUSCATE("UniversalESP")
#define ULOGE(...) __android_log_print(ANDROID_LOG_ERROR, ULOG_TAG, __VA_ARGS__)
#define ULOGD(...) __android_log_print(ANDROID_LOG_DEBUG, ULOG_TAG, __VA_ARGS__)

namespace Universal {

    // Gerenciador de Cache e Resolução Segura
    class ESP {
    private:
        struct GameOffsets {
            uintptr_t get_transform;
            uintptr_t get_position;
            uintptr_t get_main_camera;
            uintptr_t worldToScreen;
            size_t playerList;
        };

        GameOffsets offsets{};
        std::unordered_map<std::string, uintptr_t> methodCache;
        std::mutex mtx;
        std::atomic<bool> initialized{false};

        ESP() {}

        // Resolve método com cache interno
        uintptr_t getMethod(const char* assembly, const char* ns, const char* cls, const char* meth, int args = 0) {
            std::string key = std::string(assembly) + ns + cls + meth;
            if (methodCache.count(key)) return methodCache[key];

            void* addr = Il2CppGetMethodOffset(assembly, ns, cls, meth, args);
            if (!addr) {
                ULOGE("Method Not Found: %s::%s", cls, meth);
                return 0;
            }
            return methodCache[key] = reinterpret_cast<uintptr_t>(addr);
        }

    public:
        static ESP& get() {
            static ESP instance;
            return instance;
        }

        // Inicialização Segura e Universal
        void init(const char* assembly, const char* ns, const char* cls, const char* listField) {
            if (initialized.load()) return;
            std::lock_guard<std::mutex> lock(mtx);
            if (initialized.load()) return;

            ULOGD("Initializing Universal ESP v1.6 Max...");

            // Offsets Base da Unity (Sempre os mesmos para Unity/IL2CPP)
            offsets.get_transform = getMethod("UnityEngine.dll", "UnityEngine", "Component", "get_transform", 0);
            offsets.get_position = getMethod("UnityEngine.dll", "UnityEngine", "Transform", "get_position", 0);
            offsets.get_main_camera = getMethod("UnityEngine.dll", "UnityEngine", "Camera", "get_main", 0);
            offsets.worldToScreen = getMethod("UnityEngine.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1);

            // Offset da Lista de Players (Específico do Jogo)
            offsets.playerList = Il2CppGetFieldOffset(assembly, ns, cls, listField);

            initialized.store(true);
            ULOGD("Universal ESP Ready!");
        }

        // Wrapper Seguro para chamadas Unity (Previne ABI Crash em ARM64)
        template<typename T, typename... Args>
        T call(uintptr_t addr, Args... args) {
            if (!addr) return T();
            return reinterpret_cast<T(*)(Args...)>(addr)(args...);
        }

        // Funções de Interface Simples
        Vector3 getPlayerPos(void* player) {
            if (!player || !initialized.load()) return {0,0,0};
            void* transform = call<void*>(offsets.get_transform, player);
            if (!transform) return {0,0,0};
            return call<Vector3>(offsets.get_position, transform);
        }

        Vector3 worldToScreen(Vector3 worldPos) {
            if (!initialized.load()) return {0,0,0};
            void* camera = call<void*>(offsets.get_main_camera);
            if (!camera) return {0,0,0};
            return call<Vector3>(offsets.worldToScreen, camera, worldPos);
        }

        size_t getListOffset() { return offsets.playerList; }
        bool isReady() { return initialized.load(); }
    };
}

// Macros Globais para facilitar o uso no Main.cpp
#define UNIVERSAL_INIT(asm, ns, cls, fld) Universal::ESP::get().init(asm, ns, cls, fld)
#define UNIVERSAL_GET_POS(player) Universal::ESP::get().getPlayerPos(player)
#define UNIVERSAL_W2S(pos) Universal::ESP::get().worldToScreen(pos)
#define UNIVERSAL_LIST_OFFSET Universal::ESP::get().getListOffset()
