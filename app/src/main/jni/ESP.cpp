#include "ESP.h"
#include "Hooks.h"
#include <vector>
#include <string>

namespace ESP {
    bool esp_line = false;
    bool esp_box = false;
    bool esp_name = false;

    // Função dummy para simular a obtenção de uma lista de jogadores
    // Em um cenário real, esta função precisaria ser hookada ou o campo estático
    // que contém a lista de jogadores precisaria ser lido via IL2CPP.
    std::vector<Player_t*> GetDummyPlayerList() {
        std::vector<Player_t*> players;
        // Adicione jogadores fictícios para teste
        // Em um jogo real, você obteria isso do motor IL2CPP
        players.push_back(new Player_t{ (void*)0x1 }); // Dummy player 1
        players.push_back(new Player_t{ (void*)0x2 }); // Dummy player 2
        return players;
    }

    void DrawESP() {
        if (!esp_line && !esp_box && !esp_name) return;

        ImGuiIO &io = ImGui::GetIO();
        ImVec2 screen_center = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y);

        // Obter a câmera principal (se o hook estiver funcionando)
        Camera_t* mainCamera = UnityFunctions::GetMainCamera();
        if (!mainCamera) {
            // LOGD("Main Camera not found!");
            return;
        }

        // Iterar sobre a lista de jogadores (usando a dummy list por enquanto)
        std::vector<Player_t*> players = GetDummyPlayerList(); // Substituir por UnityFunctions::GetPlayerList() quando implementado

        for (Player_t* player : players) {
            if (!player) continue;

            // Obter a posição do jogador no mundo
            Vector3 worldPos = UnityFunctions::GetPlayerPosition(player);
            if (worldPos.x == 0 && worldPos.y == 0 && worldPos.z == 0) continue; // Posição inválida

            // Converter a posição do mundo para a tela
            Vector3 screenPos3D = UnityFunctions::WorldToScreenPoint(mainCamera, worldPos);
            
            // Unity WorldToScreenPoint retorna Y invertido e Z indica se está na tela
            if (screenPos3D.z < 0) continue; // Fora da tela ou atrás da câmera

            ImVec2 screenPos = ImVec2(screenPos3D.x, io.DisplaySize.y - screenPos3D.y); // Inverter Y

            ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Cor padrão: Vermelho
            // if (UnityFunctions::IsLocalPlayer(player)) color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Verde para jogador local
            // else if (UnityFunctions::IsEnemy(player)) color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Vermelho para inimigo
            // else color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Amarelo para outros

            // Desenhar ESP
            if (esp_line) {
                Drawing::DrawLine(screen_center, screenPos, color);
            }
            if (esp_box) {
                // Calcular tamanho da caixa (exemplo)
                float boxWidth = 100.0f;
                float boxHeight = 200.0f;
                Rect box_rect(screenPos.x - (boxWidth / 2), screenPos.y - boxHeight, boxWidth, boxHeight);
                Drawing::DrawBox(box_rect, color);
            }
            if (esp_name) {
                // const char* playerName = UnityFunctions::GetPlayerName(player);
                const char* playerName = "Player Name"; // Placeholder
                Drawing::DrawText2(20.0f, ImVec2(screenPos.x - (ImGui::CalcTextSize(playerName).x / 2), screenPos.y - boxHeight - 20), color, playerName);
            }
        }
    }
}
