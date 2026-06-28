#pragma once
#include <vector>
#include <string>
#include <map>
#include <Il2Cpp.h>
#include <Vector3.hpp>
#include <imgui.h>
#include "obfuscate.h"

// Macros para facilitar a busca dinâmica de offsets e métodos
#define GET_METHOD(img, ns, cls, meth, args) (uintptr_t)Il2CppGetMethodOffset(OBFUSCATE(img), OBFUSCATE(ns), OBFUSCATE(cls), OBFUSCATE(meth), args)
#define GET_FIELD(img, ns, cls, fld) Il2CppGetFieldOffset(OBFUSCATE(img), OBFUSCATE(ns), OBFUSCATE(cls), OBFUSCATE(fld))

namespace Universal {

    // Estrutura para armazenar informações dinâmicas do jogo
    struct GameData {
        const char* assembly;
        const char* namespaze;
        const char* className;
        const char* listFieldName; // Nome do campo que contém a lista de players
        uintptr_t listOffset;
        
        // Métodos comuns
        uintptr_t get_transform;
        uintptr_t get_position;
        uintptr_t worldToScreen;
        uintptr_t get_main_camera;
    };

    // Singleton para gerenciar a universalidade
    class ESP {
    public:
        static ESP& getInstance() {
            static ESP instance;
            return instance;
        }

        // Inicializa com dados específicos do jogo
        void init(const char* assembly, const char* ns, const char* cls, const char* listField) {
            data.assembly = assembly;
            data.namespaze = ns;
            data.className = cls;
            data.listFieldName = listField;

            // Busca automática de offsets base da Unity
            data.get_transform = GET_METHOD("UnityEngine.dll", "UnityEngine", "Component", "get_transform", 0);
            data.get_position = GET_METHOD("UnityEngine.dll", "UnityEngine", "Transform", "get_position", 0);
            data.worldToScreen = GET_METHOD("UnityEngine.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1);
            data.get_main_camera = GET_METHOD("UnityEngine.dll", "UnityEngine", "Camera", "get_main", 0);
            
            // Offset da lista de players no Singleton/Manager do jogo
            data.listOffset = GET_FIELD(assembly, ns, cls, listField);
        }

        // Função genérica para obter posição de qualquer objeto Unity
        Vector3 getPosition(void* component) {
            if (!component || !data.get_transform || !data.get_position) return Vector3(0,0,0);
            
            void* transform = reinterpret_cast<void*(__fastcall *)(void*)>(data.get_transform)(component);
            if (!transform) return Vector3(0,0,0);
            
            return reinterpret_cast<Vector3(__fastcall *)(void*)>(data.get_position)(transform);
        }

        // Função genérica WorldToScreen
        Vector3 worldToScreen(Vector3 worldPos) {
            void* camera = reinterpret_cast<void*(__fastcall *)()>(data.get_main_camera)();
            if (!camera || !data.worldToScreen) return Vector3(0,0,0);
            
            return reinterpret_cast<Vector3(__fastcall *)(void*, Vector3)>(data.worldToScreen)(camera, worldPos);
        }

    private:
        GameData data;
        ESP() {}
    };
}
