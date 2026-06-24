#include "ESP.h"

namespace ESP {
    bool esp_line = false;
    bool esp_box = false;
    bool esp_name = false;

    void DrawESP() {
        if (!esp_line && !esp_box && !esp_name) return;

        // Esta é uma estrutura genérica para demonstração de ESP via ImGui.
        // Em um cenário real, aqui seria a iteração sobre a lista de entidades
        // obtida dinamicamente usando os hooks e macros definidos em Hooks.h
        // Ex: WorldToScreenPoint(entity_position, &screen_pos);
        
        ImGuiIO &io = ImGui::GetIO();
        ImVec2 screen_center = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y);
        
        // Posição de uma entidade hipotética para demonstração
        ImVec2 dummy_pos = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Cor vermelha para o ESP

        if (esp_line) {
            Drawing::DrawLine(screen_center, dummy_pos, color);
        }
        if (esp_box) {
            // Exemplo de caixa de 100x200 pixels ao redor da posição da entidade
            Rect box_rect(dummy_pos.x - 50, dummy_pos.y - 100, 100, 200);
            Drawing::DrawBox(box_rect, color);
        }
        if (esp_name) {
            // Exemplo de nome "Enemy" acima da entidade
            Drawing::DrawText2(20.0f, ImVec2(dummy_pos.x - 20, dummy_pos.y - 120), color, "Enemy");
        }
    }
}
