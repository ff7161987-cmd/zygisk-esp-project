#ifndef ESP_H
#define ESP_H

#include "Struct/Drawing.h"
#include "Struct/Rect.h"
#include "ImGui/imgui.h"
#include "Hooks.h"

namespace ESP {
    extern bool esp_line;
    extern bool esp_box;
    extern bool esp_name;

    void DrawESP();
}

#endif // ESP_H
