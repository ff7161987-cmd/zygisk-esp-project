
#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <Il2Cpp.h>
#include <Tools.h>
#include <Vector3.hpp>
#include <Color.h>
#include <Struct/main.h>
#include "obfuscate.h"
#include "Unity/Unity.h"
#include "Class.h"
#include "Rect.h"
inline static int g_GlHeight, g_GlWidth;
#include "Bools.h"
#include <math.h>
#include <stdio.h>
typedef char PACKAGENAME;

struct sColor {
        
        float line[4] = {45 / 255.0f, 180 / 255.0f, 45 / 255.0f, 255 / 255.0f};
        float box[4] = {45 / 255.0f, 180 / 255.0f, 45 / 255.0f, 255 / 255.0f};
        
    };
    
    sColor Color;
   
   
bool isOutsideScreen(ImVec2 pos, ImVec2 screen) {
    if (pos.y < 0) {
        return true;
    }
    if (pos.x > screen.x) {
        return true;
    }
    if (pos.y > screen.y) {
        return true;
    }
    return pos.x < 0;
}

ImVec2 pushToScreenBorder(ImVec2 Pos, ImVec2 screen, int offset) {
    int x = (int) Pos.x;
    int y = (int) Pos.y;
    
    if (Pos.y < 0) {
        y = -offset;
    }
    
    if (Pos.x > screen.x) {
        x = (int) screen.x + offset;
    }
    
    if (Pos.y > screen.y) {
        y = (int) screen.y + offset;
    }
    
    if (Pos.x < 0) {
        x = -offset;
    }
    return ImVec2(x, y);
}

static float tm = 127 / 255.f;

void DrawCircleHealth(ImVec2 position, int health, int max_health, float radius) {
    float a_max = ((3.14159265359f * 2.0f));
    ImU32 healthColor = IM_COL32(45, 180, 45, 255);
    if (health <= (max_health * 0.6)) {
        healthColor = IM_COL32(180, 180, 45, 255);
    }
    if (health < (max_health * 0.3)) {
        healthColor = IM_COL32(180, 45, 45, 255);
    }
    ImGui::GetForegroundDrawList()->PathArcTo(position, radius, (-(a_max / 4.0f)) + (a_max / max_health) * (max_health - health), a_max - (a_max / 4.0f));
    ImGui::GetForegroundDrawList()->PathStroke(healthColor, ImDrawFlags_None, 4);
}



void drawText2(int x, int y, long color, const char *str, float size) {
        ImGui::GetBackgroundDrawList()->AddText(NULL, size, ImVec2(x, y), color, str);
    }


void DrawRectFilled(int x, int y, int w, int h, ImColor color) {
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h),
                                               color, 0, 0);
}


void DrawRectFilledHealth(int x, int y, int w, int h, ImVec4 color) {
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h),
                                               ImGui::ColorConvertFloat4ToU32(color), 0, 0);
}


void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, int T) {
    ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3),
                                                   color);
}
bool IsVisible(void * player){
    if(player != NULL) {
        void *hitObj = NULL;
        

        
        Vector3 cameraLocation = Transform_GetPosition(Component_GetTransform(Camera_main()));
        Vector3 headLocation = Transform_GetPosition(Component_GetTransform(Player_GetHeadCollider(player)));
            return !Physics_Raycast(cameraLocation, headLocation, 12, &hitObj);
    }
    return false;
}


void DrawHealths(Rect box, float entityHealth, float maxHealth, int TeamID, const char* name, long color) {
    float x = box.x - (140 - box.w) / 2;
    float y = box.y;
    char dis[20];
    // sprintf(dis, "%0.fM", d);

    char TeamText[20];
    sprintf(TeamText, "%d", (int)TeamID);
    ImVec4 HPColor = ImVec4{1, 1, 0, 127 / 255.f}; // Màu mặc định cho máu

    if (entityHealth < maxHealth) {
        float healthPercentage = entityHealth / maxHealth;

        if (healthPercentage < 0.3f) {
            HPColor = ImVec4{0.5f, 0.0f, 0.0f, 127 / 255.f}; // Màu đỏ cho máu thấp
        } else if (healthPercentage < 0.6f) {
            HPColor = ImVec4{1, 0, 0, 127 / 255.f}; // Màu cam cho máu trung bình
        }
    }

    tm = 100.f / 255.f;
    DrawRectFilled(x - strlen(name), y - 41, 120 + strlen(name) * 2, 20, color); // Đám mây nền cho tên người chơi
    tm = 255.f / 255.f;
    DrawRectFilled(x - strlen(name), box.y - 41, 25, 20, color);
    if (strlen(TeamText) < 2) {
        drawText2(x + 6 - strlen(name), box.y - 42, ImColor(255,255,255),
                       TeamText, 18.943);//队伍ID
    } else {
        drawText2(x - strlen(name), box.y - 42, ImColor(255,255,255),
                       TeamText, 18.943);//队伍ID
    }
    drawText2(x + 28 - strlen(name), y - 43,
                   ImColor(255,255,255), name, 18.943);//玩家名称
    float maxWidth = 120; // Độ dài tối đa của thanh máu
    float healthBarWidth = entityHealth * maxWidth / maxHealth; // Độ dài thanh máu dựa trên tỷ lệ máu hiện tại và máu tối đa
    DrawRectFilledHealth(x - maxWidth / 2, y - 18, healthBarWidth, 8, HPColor);// cập nhật vị trí và kích thước của thanh máu
    drawText2(x + 125 + strlen(name), y - 43, ImColor(255,255,255), dis,18.943);//距离显示
    DrawTriangle(box.x + box.w / 2 - 10, y - 8, box.x + box.w / 2 + 15 - 10, y - 8,
                 box.x + box.w / 2 - 2, y, ImColor(255,255,255), 1);//第一个左边 第二个右边 第三个底边
}


static float SetFieldOfView = 0, GetFieldOfView = 0;

void DroneView() {
    if (GetFieldOfView == 0) {
        GetFieldOfView = get_fieldOfView();
    }
    if (SetFieldOfView > 0 && GetFieldOfView != 0) {
        set_fieldOfView((float)GetFieldOfView + SetFieldOfView);
    }
    if (SetFieldOfView == 0 && GetFieldOfView != 0) {
        set_fieldOfView((float)GetFieldOfView);
    }
}

Quaternion GetRotationToTheLocation(Vector3 Target, float Height, Vector3 MyEnemy) {
return Quaternion::LookRotation((Target + Vector3(0, Height, 0)) - MyEnemy, Vector3(0, 1, 0));
}



void *GetClosestEnemy() {
    float shortestDistance = 9999;
    void* closestEnemy = NULL;   
    void* get_MatchGame = Curent_Match();
    void* LocalPlayer = GetLocalPlayer(get_MatchGame);
    if (LocalPlayer != NULL && get_MatchGame != NULL && Enable && get_MatchGame) {
        monoDictionary<uint8_t *, void **> *players = *(monoDictionary<uint8_t*, void **> **)((long)get_MatchGame + m_ShortIDToPlayers);
        for (int u = 0; u < players->getNumValues(); u++) {
            void* Player = players->getValues()[u]; 
            if (Player != NULL && !get_isLocalTeam(Player) && !get_IsDieing(Player) && get_isVisible(Player) && get_MaxHP(Player)) {    
                Vector3 PlayerPos = getPosition(Player);
                Vector3 LocalPlayerPos = getPosition(LocalPlayer);
                Vector3 pos2 = WorldToScreenPoint(Camera_main(), PlayerPos);
                bool isFov1 = isFov(Vector3(pos2.x, pos2.y), Vector3(g_GlWidth / 2, g_GlHeight / 2), Fov_Aim);
                float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
                if (distance < Aimdis) {
                    Vector3 targetDir = Vector3::Normalized(PlayerPos - LocalPlayerPos);
                    float angle = Vector3::Angle(targetDir, GetForward(Component_GetTransform(Camera_main()))) * 100.0;             
            
                    if (angle <= Fov_Aim && isFov1 && angle < shortestDistance) {
                        shortestDistance = angle;
                        closestEnemy = Player;
                    }
                }
            }
        }
    }
    return closestEnemy;
}
float (*get_deltaTime)();
float smoothSpeedAIM = 5; 
void *MyPlayer = NULL;
bool Drone;
const float RADAR_SIZE = 200.0f; 
const ImVec2 RADAR_CENTER(100.0f, 100.0f); 
float x,y;
float enemy1 = 2.0f;
bool RAMA_MODZ = true;
bool RamaModz = true;
bool RenderWatermark = true;
#include <string>
ImVec4 colorTextBackground = ImVec4(0, 0, 0, 255); 
ImVec4 colorBox = ImVec4(1.0f, 0.0f, 0.0f, 0.5f);       
ImVec4 colorHealth = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); 
ImVec4 colorDistance = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); 
ImVec4 colorLine = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);        
ImVec4 colorHexagono = ImVec4(150 / 255.0f, 0, 255 / 255.0f, 1.0f);
ImVec4 colorMenuText = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
void* CurrentMatch = nullptr;
void* closestEnemy = nullptr;
void* LocalPlayer = nullptr;


float aimSmoothness = 0.1f;

inline void DrawAIMBOT(float screenWidth, float screenHeight) {
     
}

inline void DrawESP(float screenWidth, float screenHeight) {
 ImDrawList*draw = ImGui::GetBackgroundDrawList();
    if (!draw) return;
if (Aimbot) {
void* CurrentMatch = Curent_Match();
void* closestEnemy = GetClosestEnemy();
void* LocalPlayer = GetLocalPlayer(CurrentMatch);

if (closestEnemy != NULL && LocalPlayer != NULL && CurrentMatch != NULL) {
    Vector3 EnemyLocation = GetHeadPosition(closestEnemy);
    Vector3 PlayerLocation = CameraMain(LocalPlayer);
    Quaternion PlayerLook = GetRotationToLocation(EnemyLocation, 0.1f, PlayerLocation);
    bool IsScopeOn = get_IsSighting(LocalPlayer);
    bool IsFiring = get_IsFiring(LocalPlayer);
    if (AimWhen == 0) {
        set_aim(LocalPlayer, PlayerLook);
    }
    else if (AimWhen == 1 && IsFiring) {
        set_aim(LocalPlayer, PlayerLook);
    }
    else if (AimWhen == 2 && IsScopeOn) {
        set_aim(LocalPlayer, PlayerLook);
    }
    
}
}
if (Enable) {        
        int enemyIndex = 0; 
        void* current_Match = Curent_Match();
        void* local_player = GetLocalPlayer(current_Match);

        if (local_player && current_Match) {
            auto* players = *(monoDictionary<uint8_t*, void**>**)((long)current_Match + m_ShortIDToPlayers);
            void* camera = Camera_main();

            if (players && camera) {
                for (int u = 0; u < players->getNumValues(); u++) {
                    void* closestEnemy = players->getValues()[u];
                  if (closestEnemy != local_player && closestEnemy && get_isVisible(closestEnemy) && !get_isLocalTeam(closestEnemy, true)) {enemyIndex++;
                        Vector3 Toepos = getPosition(closestEnemy);
                        Vector3 Toeposi = WorldToScreenPoint(camera, Toepos);
                        if (Toeposi.z < 1) continue;
                        Vector3 HeadPos = getPosition(closestEnemy) + Vector3(0, 1.9f, 0);
                        Vector3 HeadPosition = WorldToScreenPoint(camera, HeadPos);
                        if (HeadPosition.z < 1) continue;
                        {float cx = screenWidth / 2.0f;
                        float cy = screenHeight / 2.0f;
                        float radius = Fov_Aim;
                        int segments = 100;
                        float thickness = 1.5f;
                        float time = ImGui::GetTime() * 0.5f;
                        for (int i = 0; i < segments; i++) {
                        float theta1 = (i / (float)segments) * 2.0f * IM_PI;
                        float theta2 = ((i + 1) / (float)segments) * 2.0f * IM_PI;
                        float x1 = cx + cosf(theta1) * radius;
                        float y1 = cy + sinf(theta1) * radius;
                        float x2 = cx + cosf(theta2) * radius;
                        float y2 = cy + sinf(theta2) * radius;
                        float hue = fmodf((i / (float)segments) + time, 1.0f);
                        ImColor color = ImColor::HSV(hue, 1.0f, 1.0f);
                        draw->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);}}
                        float distance = Vector3::Distance(getPosition(local_player), Toepos);
                        float Height = abs(HeadPosition.y - Toeposi.y) * (1.2 / 1.1);
                        float Width = Height * 0.50f;
                        Rect rect = Rect(HeadPosition.x - Width / 2.f, screenHeight - HeadPosition.y, Width, Height);
if (Config.ESP.Health) {           
             long clr = ImColor(0,255,0,255);               
                                
              if (GetHp(closestEnemy) <= (get_MaxHP(closestEnemy) * 0.6)) {
                clr = ImColor(255, 255, 0, 255);
          }
              
              if (GetHp(closestEnemy) < (get_MaxHP(closestEnemy) * 0.3)) {
                clr = ImColor(255, 0, 0, 255);
        }
             int xx = rect.x + rect.w + 2;
             int yy = rect.y;
            draw->AddRectFilled(ImVec2(xx, yy), ImVec2(xx + 5, yy + rect.h), ImColor(0, 0, 0, 255));        
            draw->AddRectFilled(ImVec2(xx + 1, yy + rect.h - (rect.h * ((float)GetHp(closestEnemy) / get_MaxHP(closestEnemy)))), ImVec2(xx + 4, yy + rect.h),clr);                                        
        }
if (Config.ESP.Line) {
    float time = ImGui::GetTime() * 0.5f;
    float hue = fmod(time, 1.0f);
    
    ImVec4 rgb;
    ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, rgb.x, rgb.y, rgb.z);

    draw->AddLine(
        ImVec2(screenWidth / 2.0f, 10), 
        ImVec2(HeadPosition.x, screenHeight - HeadPosition.y), 
        ImColor(rgb.x, rgb.y, rgb.z, 1.0f), 
        1.7f
    );
}
if (Config.ESP.Hexagono) {
        ImVec2 center(screenWidth / 2.0f, screenHeight / 2.0f);
        float radius = 30.0f;
        float angleOffset = PI / 2.0f;

        ImVec2 vertices[6];
        for (int i = 0; i < 6; ++i) {
            float angle = angleOffset + i * (2 * PI / 6);
            vertices[i] = ImVec2(center.x + cos(angle) * radius, center.y + sin(angle) * radius);
        }

        draw->AddPolyline(vertices, 6, ImColor(colorHexagono), true, 1.7f);
    }
if (Config.ESP.Box) {
    float time = ImGui::GetTime() * 0.5f; 
    float hue = fmod(time, 1.0f);
    ImVec4 rgb;
    ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, rgb.x, rgb.y, rgb.z);

    draw->AddRect(
        ImVec2(rect.x, rect.y), 
        ImVec2(rect.x + rect.w, rect.y + rect.h), 
        ImColor(rgb.x, rgb.y, rgb.z, 1.0f), 
        5.0f, 0, 1.7f
    );
}
if (Config.ESP.RamaMods) {
    monoString* Nick = get_NickName(closestEnemy);
    std::string name;
    if (Nick != nullptr) {
        int len = Nick->getLength();
        name.reserve(len);
        for (int i = 0; i < len; i++) {
            char c = get_Chars(Nick, i);
            name += (isascii(c) && isprint(c)) ? c : '?';
        }
    }
    std::string teamText = std::to_string(enemyIndex);
    std::string distText = std::to_string((int)distance) + "M";
    ImFont* font = ImGui::GetFont();
    float fontScale = 0.7f;
    float fontSize = font->FontSize * fontScale;
    ImVec2 teamSize = ImGui::CalcTextSize(teamText.c_str());
    teamSize.x *= fontScale;
    teamSize.y *= fontScale;
    ImVec2 nameSize = ImGui::CalcTextSize(name.c_str());
    nameSize.x *= fontScale;
    nameSize.y *= fontScale;
    ImVec2 distSize = ImGui::CalcTextSize(distText.c_str());
    distSize.x *= fontScale;
    distSize.y *= fontScale;
    float spacing = 2.0f;
    float padding = 2.0f;
    float totalWidth = teamSize.x + spacing + nameSize.x + spacing + distSize.x + padding * 6;
    float posX = rect.x + rect.w * 0.5f - totalWidth * 0.5f;
    float posY = rect.y - nameSize.y - 5.0f;
    draw->AddRectFilled(
        ImVec2(posX, posY - 2),
        ImVec2(posX + teamSize.x + padding * 2, posY + teamSize.y + 2),
        ImColor(0, 160, 0, 180), 2.0f
    );
    draw->AddText(font, fontSize, ImVec2(posX + padding, posY), ImColor(255, 255, 255), teamText.c_str());
    float nameX = posX + teamSize.x + spacing + padding * 2;
    draw->AddRectFilled(
        ImVec2(nameX, posY - 2),
        ImVec2(nameX + nameSize.x + padding * 2, posY + nameSize.y + 2),
        ImColor(0, 0, 0, 160), 2.0f
    );
    draw->AddText(font, fontSize, ImVec2(nameX + padding, posY), ImColor(255, 255, 255), name.c_str());
    float distX = nameX + nameSize.x + spacing + padding * 2;
    draw->AddRectFilled(
        ImVec2(distX, posY - 2),
        ImVec2(distX + distSize.x + padding * 2, posY + distSize.y + 2),
        ImColor(0, 0, 0, 160), 2.0f
    );
    draw->AddText(font, fontSize, ImVec2(distX + padding, posY), ImColor(0, 255, 0), distText.c_str());}}}}}}
  if (RenderWatermark){
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 wmSize = ImVec2(250, 40);
    ImVec2 wmPos = ImVec2((displaySize.x - wmSize.x) / 2, 20); 
    ImVec2 wmEnd = ImVec2(wmPos.x + wmSize.x, wmPos.y + wmSize.y);
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        wmPos, wmEnd,
        IM_COL32(25, 25, 25, 220),
        6.0f);
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(wmPos.x, wmPos.y), 
        ImVec2(wmEnd.x, wmPos.y + 3), 
        IM_COL32(200, 0, 255, 255),
        2.0f);
    for (int i = 1; i <= 6; i++) {
        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(wmPos.x, wmPos.y - i),
            ImVec2(wmEnd.x, wmPos.y + 3 + i),
            IM_COL32(200, 0, 255, 40 - i * 6),
            2.0f);}
    std::string watermarkText = "RAMA MODZ";
    ImVec2 textSize = ImGui::CalcTextSize(watermarkText.c_str());
    ImVec2 textPos = ImVec2(
        wmPos.x + (wmSize.x - textSize.x) / 2,
        wmPos.y + (wmSize.y - textSize.y) / 2);
    ImGui::GetBackgroundDrawList()->AddText(
        textPos,
        IM_COL32(255, 255, 255, 255),
        watermarkText.c_str());}}
bool SilentAim;
void (*ori_bypass_ramamodz)(void *, float, float);
void Modify_Bypass_RamaModz(void *_this, float a1, float a2) {
if (_this != nullptr) {
if(Aimbot) {
if (SilentAim) {
void *CurrentMatch = Curent_Match();
if (CurrentMatch != NULL) {
void* LocalPlayer = GetLocalPlayer(CurrentMatch);
if (LocalPlayer != NULL)  {
bool visible = get_isVisible(LocalPlayer);
if (visible) {
if (get_IsFiring(GetLocalPlayer(CurrentMatch))) {
return;
} else {
ori_bypass_ramamodz(_this, a1, a2);}}}}}}ori_bypass_ramamodz(_this, a1, a2);}}
bool Guest;
bool (*ResetGuest)(void* _this);
bool _ResetGuest(void* _this) {
if (Guest) {return true;}
return ResetGuest(_this);}
bool camxa;
float (*GetCameraHeightRateValue)(void *player);
float _GetCameraHeightRateValue(void *player) {
if (player != nullptr) {
if (camxa) {return 4.0f;}}
return GetCameraHeightRateValue(player);}
bool(*Bypass)(void* bypass); 
bool _Bypass(void* bypass) {return false;}



