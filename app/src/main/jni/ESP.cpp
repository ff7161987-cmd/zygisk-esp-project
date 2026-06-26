#include "ESP.h"
#include "Hooks.h"
#include "Struct/Class.h" // Inclui Class.h para acesso às macros e wrappers
#include <vector>
#include <string>

namespace ESP {
    bool esp_line = false;
    bool esp_box = false;
    bool esp_name = false;

    // Estrutura para representar um jogador, usando o void* para o objeto real do jogo
    struct GamePlayer {
        void* object;
        // Adicione outros campos relevantes se necessário, como ponteiro para o transform, etc.
    };

    // Função dummy para simular a obtenção de uma lista de jogadores
    // Em um cenário real, esta função precisaria ser hookada ou o campo estático
    // que contém a lista de jogadores precisaria ser lido via IL2CPP.
    std::vector<GamePlayer> GetGamePlayerList() {
        std::vector<GamePlayer> players;
        // Adicione jogadores fictícios para teste
        // Em um jogo real, você obteria isso do motor IL2CPP
        // Exemplo: iterar sobre uma lista de objetos de jogo e filtrar jogadores
        players.push_back({ (void*)0x1 }); // Dummy player 1
        players.push_back({ (void*)0x2 }); // Dummy player 2
        return players;
    }

    void DrawESP() {
        if (!esp_line && !esp_box && !esp_name) return;

        ImGuiIO &io = ImGui::GetIO();
        ImVec2 screen_center = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y);

        // Obter a câmera principal usando o wrapper de Class.h
        void* mainCamera = Camera_main();
        if (!mainCamera) {
            // LOGD("Main Camera not found!");
            return;
        }

        // Iterar sobre a lista de jogadores (usando a dummy list por enquanto)
        std::vector<GamePlayer> players = GetGamePlayerList();

        for (GamePlayer gamePlayer : players) {
            if (!gamePlayer.object) continue;

            // Obter o transform do jogador
            void* playerTransform = Component_GetTransform(gamePlayer.object);
            if (!playerTransform) continue;

            // Obter a posição do jogador no mundo usando o wrapper de Class.h
            Vvector3 worldPos = get_position(playerTransform);
            if (worldPos.X == 0 && worldPos.Y == 0 && worldPos.Z == 0) continue; // Posição inválida

            // Converter a posição do mundo para a tela usando o wrapper de Class.h
            Vvector3 screenPos3D = WorldToScreenPoint(mainCamera, worldPos);
            
            // Unity WorldToScreenPoint retorna Y invertido e Z indica se está na tela
            if (screenPos3D.Z < 0) continue; // Fora da tela ou atrás da câmera

            ImVec2 screenPos = ImVec2(screenPos3D.X, io.DisplaySize.y - screenPos3D.Y); // Inverter Y

            ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Cor padrão: Vermelho
            
            // Exemplo de como usar as funções de Class.h para determinar cor
            // if (IsLocal(gamePlayer.object)) color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Verde para jogador local
            // else if (!get_isLocalTeam(gamePlayer.object)) color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Vermelho para inimigo (não é local e não é do time)
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
                monoString* nameMonoStr = get_NickName(gamePlayer.object);
                const char* playerName = "Player Name"; // Placeholder
                if (nameMonoStr) {
                    playerName = nameMonoStr->CString();
                }
                Drawing::DrawText2(20.0f, ImVec2(screenPos.x - (ImGui::CalcTextSize(playerName).x / 2), screenPos.y - boxHeight - 20), color, playerName);
            }
        }
    }
}
