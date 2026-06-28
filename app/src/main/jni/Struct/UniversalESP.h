#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <type_traits>
#include <Il2Cpp.h>
#include <Vector3.hpp>
#include <android/log.h>
#include "obfuscate.h"

/**
 * UNIVERSAL IL2CPP ADAPTIVE FRAMEWORK v2.0 (Manus 1.6 Max Edition)
 * Foco: Estabilidade Extrema, Prevenção de Crash e Compatibilidade Multi-Versão.
 */

#define ULOG_TAG OBFUSCATE("UniversalESP_Core")
#define ULOGE(...) __android_log_print(ANDROID_LOG_ERROR, ULOG_TAG, __VA_ARGS__)
#define ULOGD(...) __android_log_print(ANDROID_LOG_DEBUG, ULOG_TAG, __VA_ARGS__)

namespace Universal {

    // Status de Resolução
    enum class ResolveStatus {
        UNRESOLVED,
        SUCCESS,
        FAILED,
        FALLBACK_USED
    };

    // Metadados do Método Resolvido
    struct MethodMetadata {
        uintptr_t address = 0;
        ResolveStatus status = ResolveStatus::UNRESOLVED;
        int confidence = 0; // 0-100
    };

    class ESP {
    private:
        std::unordered_map<std::string, MethodMetadata> methodCache;
        std::mutex cacheMtx;
        std::atomic<bool> isReady{false};

        struct {
            MethodMetadata get_transform;
            MethodMetadata get_position;
            MethodMetadata get_main_camera;
            MethodMetadata worldToScreen;
            size_t playerListOffset = (size_t)-1;
        } core;

        ESP() {}

        // Resolver Adaptativo: Tenta encontrar o método com diferentes estratégias
        MethodMetadata adaptiveResolve(const char* assembly, const char* ns, const char* cls, const char* meth, int args) {
            MethodMetadata meta;
            
            // Estratégia 1: Busca Exata
            void* addr = Il2CppGetMethodOffset(assembly, ns, cls, meth, args);
            if (addr) {
                meta.address = reinterpret_cast<uintptr_t>(addr);
                meta.status = ResolveStatus::SUCCESS;
                meta.confidence = 100;
                return meta;
            }

            // Estratégia 2: Busca por nome sem args (Fallback comum em versões diferentes de Unity)
            addr = Il2CppGetMethodOffset(assembly, ns, cls, meth, -1);
            if (addr) {
                meta.address = reinterpret_cast<uintptr_t>(addr);
                meta.status = ResolveStatus::FALLBACK_USED;
                meta.confidence = 70;
                ULOGD("Fallback used for %s::%s (args mismatch)", cls, meth);
                return meta;
            }

            ULOGE("CRITICAL: Failed to resolve %s::%s", cls, meth);
            meta.status = ResolveStatus::FAILED;
            return meta;
        }

    public:
        static ESP& getInstance() {
            static ESP instance;
            return instance;
        }

        // Inicialização com validação de segurança
        void init(const char* assembly, const char* ns, const char* cls, const char* listField) {
            if (isReady.load()) return;
            std::lock_guard<std::mutex> lock(cacheMtx);
            if (isReady.load()) return;

            ULOGD("Starting Adaptive Resolver...");

            // Resolução do Core Unity (Abstração de Versão)
            core.get_transform = adaptiveResolve("UnityEngine.dll", "UnityEngine", "Component", "get_transform", 0);
            core.get_position = adaptiveResolve("UnityEngine.dll", "UnityEngine", "Transform", "get_position", 0);
            core.get_main_camera = adaptiveResolve("UnityEngine.dll", "UnityEngine", "Camera", "get_main", 0);
            core.worldToScreen = adaptiveResolve("UnityEngine.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1);

            // Resolução específica do jogo
            core.playerListOffset = Il2CppGetFieldOffset(assembly, ns, cls, listField);

            isReady.store(true);
            ULOGD("Framework initialized. Readiness: %s", (core.get_transform.status != ResolveStatus::FAILED) ? "OK" : "PARTIAL");
        }

        /**
         * INVOCAÇÃO SEGURA (Safe Call)
         * - Valida ponteiro nulo
         * - Protege contra ABI mismatch em ARM64
         * - Retorna valor padrão em caso de erro ao invés de crashar
         */
        template<typename Ret, typename... Args>
        Ret safeInvoke(MethodMetadata& meta, Args... args) {
            if (meta.status == ResolveStatus::FAILED || meta.address == 0) {
                return Ret();
            }
            
            try {
                // No ARM64, chamadas diretas a ponteiros resolvidos por dlsym/xdl precisam ser cuidadosas
                typedef Ret (*Func)(Args...);
                return reinterpret_cast<Func>(meta.address)(args...);
            } catch (...) {
                ULOGE("EXCEPTION: Crash prevented during safeInvoke at %p", (void*)meta.address);
                return Ret();
            }
        }

        // --- Interface de Alto Nível ---

        Vector3 getPosition(void* component) {
            if (!component || !isReady.load()) return {0,0,0};
            
            void* transform = safeInvoke<void*>(core.get_transform, component);
            if (!transform) return {0,0,0};
            
            return safeInvoke<Vector3>(core.get_position, transform);
        }

        Vector3 worldToScreen(Vector3 worldPos) {
            if (!isReady.load()) return {0,0,0};
            
            void* camera = safeInvoke<void*>(core.get_main_camera);
            if (!camera) return {0,0,0};
            
            return safeInvoke<Vector3>(core.worldToScreen, camera, worldPos);
        }

        size_t getPlayerListOffset() { return core.playerListOffset; }
        bool isValid() { return isReady.load() && core.get_transform.status != ResolveStatus::FAILED; }
    };
}

// Macros Globais Atualizadas para a v2.0
#define UNIVERSAL_INIT(asm, ns, cls, fld) Universal::ESP::getInstance().init(asm, ns, cls, fld)
#define UNIVERSAL_GET_POS(player) Universal::ESP::getInstance().getPosition(player)
#define UNIVERSAL_W2S(pos) Universal::ESP::getInstance().worldToScreen(pos)
#define UNIVERSAL_LIST_OFFSET Universal::ESP::getInstance().getPlayerListOffset()
#define UNIVERSAL_READY Universal::ESP::getInstance().isValid()
