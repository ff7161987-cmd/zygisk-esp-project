#pragma once
#include "Runtime_Core.hpp"

namespace Universal {

    class Framework {
    public:
        // Interface simplificada para o usuário final
        static bool Setup(const char* assembly, const char* ns, const char* cls, const char* listField) {
            return Core::getInstance().init(assembly, ns, cls, listField);
        }

        static Vector3 GetPlayerPosition(void* player) {
            if (!Core::getInstance().isReady()) return Vector3(0,0,0);
            void* transform = Unity::getInstance().getTransform(player);
            return Unity::getInstance().getPosition(transform);
        }

        static Vector3 WorldToScreen(Vector3 worldPos) {
            if (!Core::getInstance().isReady()) return Vector3(0,0,0);
            return Unity::getInstance().worldToScreen(worldPos);
        }
        
        static size_t GetPlayerListOffset() {
            return Core::getInstance().getListOffset();
        }
    };
}
