#ifndef ESP_H
#define ESP_H

#include "Struct/Drawing.h"
#include "Struct/Rect.h"
#include "ImGui/imgui.h"
#include "Hooks.h"
#include "Struct/Class.h" // Inclui Class.h para acesso às macros e wrappers
#include <vector>

namespace ESP {
    extern bool esp_line;
    extern bool esp_box;
    extern bool esp_name;

    // Estrutura para representar um jogador, usando o void* para o objeto real do jogo
    struct GamePlayer {
        void* object;
        // Adicione outros campos relevantes se necessário, como ponteiro para o transform, etc.
    };

    void DrawESP();
}

#endif // ESP_H
