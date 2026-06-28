#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include <Il2Cpp.h>
#include <android/log.h>
#include "../obfuscate.h"

#define LOG_TAG "UniversalESP"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

namespace Universal {

    class Resolver {
    public:
        static Resolver& getInstance() {
            static Resolver instance;
            return instance;
        }

        // Resolve e armazena em cache o offset do método
        uintptr_t resolveMethod(const char* assembly, const char* ns, const char* cls, const char* method, int argsCount = 0) {
            std::string key = std::string(assembly) + ns + cls + method + std::to_string(argsCount);
            
            std::lock_guard<std::mutex> lock(cacheMutex);
            if (methodCache.count(key)) {
                return methodCache[key];
            }

            void* offset = Il2CppGetMethodOffset(assembly, ns, cls, method, argsCount);
            if (!offset) {
                LOGE("Failed to resolve method: %s::%s", cls, method);
                return 0;
            }

            uintptr_t addr = reinterpret_cast<uintptr_t>(offset);
            methodCache[key] = addr;
            LOGD("Resolved method %s::%s at %p", cls, method, (void*)addr);
            return addr;
        }

        // Resolve e armazena em cache o offset do campo
        size_t resolveField(const char* assembly, const char* ns, const char* cls, const char* field) {
            std::string key = std::string(assembly) + ns + cls + field + "_field";
            
            std::lock_guard<std::mutex> lock(cacheMutex);
            if (fieldCache.count(key)) {
                return fieldCache[key];
            }

            size_t offset = Il2CppGetFieldOffset(assembly, ns, cls, field);
            if (offset == (size_t)-1) {
                LOGE("Failed to resolve field: %s::%s", cls, field);
                return (size_t)-1;
            }

            fieldCache[key] = offset;
            LOGD("Resolved field %s::%s at offset %zu", cls, field, offset);
            return offset;
        }

        // Wrapper seguro para chamadas de função
        template<typename Ret, typename... Args>
        Ret safeCall(uintptr_t methodAddr, Args... args) {
            if (!methodAddr) return Ret();
            typedef Ret (*Func)(Args...);
            return reinterpret_cast<Func>(methodAddr)(args...);
        }

    private:
        std::unordered_map<std::string, uintptr_t> methodCache;
        std::unordered_map<std::string, size_t> fieldCache;
        std::mutex cacheMutex;
        Resolver() {}
    };
}
