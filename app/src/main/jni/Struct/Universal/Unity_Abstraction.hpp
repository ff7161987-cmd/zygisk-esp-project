#pragma once
#include "IL2CPP_Resolver.hpp"
#include <Vector3.hpp>

namespace Universal {

    class Unity {
    public:
        struct Offsets {
            uintptr_t get_transform;
            uintptr_t get_position;
            uintptr_t get_main_camera;
            uintptr_t worldToScreen;
        };

        static Unity& getInstance() {
            static Unity instance;
            return instance;
        }

        void initBaseOffsets() {
            auto& res = Resolver::getInstance();
            offsets.get_transform = res.resolveMethod("UnityEngine.dll", "UnityEngine", "Component", "get_transform", 0);
            offsets.get_position = res.resolveMethod("UnityEngine.dll", "UnityEngine", "Transform", "get_position", 0);
            offsets.get_main_camera = res.resolveMethod("UnityEngine.dll", "UnityEngine", "Camera", "get_main", 0);
            offsets.worldToScreen = res.resolveMethod("UnityEngine.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1);
        }

        // Abstração segura para obter Transform
        void* getTransform(void* component) {
            if (!component) return nullptr;
            return Resolver::getInstance().safeCall<void*>(offsets.get_transform, component);
        }

        // Abstração segura para obter Posição
        Vector3 getPosition(void* transform) {
            if (!transform) return Vector3(0,0,0);
            return Resolver::getInstance().safeCall<Vector3>(offsets.get_position, transform);
        }

        // Abstração segura para WorldToScreen
        Vector3 worldToScreen(Vector3 worldPos) {
            void* camera = Resolver::getInstance().safeCall<void*>(offsets.get_main_camera);
            if (!camera) return Vector3(0,0,0);
            return Resolver::getInstance().safeCall<Vector3>(offsets.worldToScreen, camera, worldPos);
        }

    private:
        Offsets offsets;
        Unity() {}
    };
}
