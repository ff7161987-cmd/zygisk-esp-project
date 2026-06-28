#include <android/log.h>
#include <unistd.h>
#include <thread>
#include <limits>
#include <xdl.h>
#include <KittyUtils.h>
#include <KittyMemory.h>
#include <Il2Cpp.h>
#include <dobby.h>
#include "Struct/Gui.hpp"
#include <Struct/main.h>
#include "fonts/FontAwesome6_solid.h"
#include "Toggle.h"
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

void hack();

class MyModule : public zygisk::ModuleBase {
 public:
  void onLoad(Api *api, JNIEnv *env) override {
    this->api_ = api;
    this->env_ = env;
    genv = env;
  }
  void preAppSpecialize(AppSpecializeArgs *args) override {
    static constexpr const char *packages[] = {
        "com.dts.freefireth",
        "com.dts.freefiremax"
    };
    const char *process = env_->GetStringUTFChars(args->nice_name, nullptr);
    for (const auto *package: packages) {
      is_game_ = (strcmp(process, package) == 0);
      if (is_game_) break;
    }
    env_->ReleaseStringUTFChars(args->nice_name, process);
  }
  void postAppSpecialize(const AppSpecializeArgs *args) override {
    if (is_game_) {
      genv->GetJavaVM(&jvm);
      std::thread{hack}.detach();
    }
  }
 private:
  Api *api_;
  JNIEnv *env_;
  bool is_game_ = false;
};

void SetElegantBlackAndBlueTheme()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuiStyle* style = &ImGui::GetStyle();
    style->Alpha = 1.0f;
    style->WindowRounding = 10.0f;
    style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style->FrameRounding = 10.0f;
    style->FramePadding = ImVec2(4, 7);
    style->ScrollbarSize = 25;
    style->Colors[ImGuiCol_Text]                 = ImColor(255, 255, 255, 255);
    style->Colors[ImGuiCol_TextDisabled]         = ImColor(130, 130, 130, 255);
    style->Colors[ImGuiCol_WindowBg]             = ImColor(10, 20, 40, 255);
    style->Colors[ImGuiCol_ChildBg]              = ImColor(10, 20, 40, 200);
    style->Colors[ImGuiCol_PopupBg]              = ImColor(20, 30, 60, 240);
    style->Colors[ImGuiCol_Border]               = ImColor(50, 50, 70, 180);
    style->Colors[ImGuiCol_BorderShadow]         = ImVec4(0, 0, 0, 0);
    style->Colors[ImGuiCol_FrameBg]              = ImColor(50, 90, 140, 160);
    style->Colors[ImGuiCol_FrameBgHovered]       = ImColor(70, 110, 160, 200);
    style->Colors[ImGuiCol_FrameBgActive]        = ImColor(90, 130, 180, 255);
    style->Colors[ImGuiCol_TitleBg]              = ImColor(0, 50, 100, 255);
    style->Colors[ImGuiCol_TitleBgActive]        = ImColor(0, 80, 160, 255);
    style->Colors[ImGuiCol_TitleBgCollapsed]     = ImColor(0, 50, 100, 130);
    style->Colors[ImGuiCol_MenuBarBg]            = ImColor(20, 40, 60, 255);
    style->Colors[ImGuiCol_ScrollbarBg]          = ImColor(10, 20, 30, 160);
    style->Colors[ImGuiCol_ScrollbarGrab]        = ImColor(60, 90, 130, 255);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(70, 110, 150, 255);
    style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImColor(90, 130, 180, 255);
    style->Colors[ImGuiCol_CheckMark]            = ImColor(80, 150, 255, 255);
    style->Colors[ImGuiCol_SliderGrab]           = ImColor(60, 120, 200, 255);
    style->Colors[ImGuiCol_SliderGrabActive]     = ImColor(80, 150, 255, 255);
    style->Colors[ImGuiCol_Button]               = ImColor(0, 100, 200, 200);
    style->Colors[ImGuiCol_ButtonHovered]        = ImColor(0, 120, 255, 255);
    style->Colors[ImGuiCol_ButtonActive]         = ImColor(0, 100, 220, 255);
    style->Colors[ImGuiCol_Header]               = ImColor(40, 100, 200, 160);
    style->Colors[ImGuiCol_HeaderHovered]        = ImColor(50, 120, 240, 200);
    style->Colors[ImGuiCol_HeaderActive]         = ImColor(30, 90, 180, 255);
    style->Colors[ImGuiCol_Separator]            = ImColor(255, 255, 255, 160);
    style->Colors[ImGuiCol_SeparatorHovered]     = ImColor(255, 255, 255, 200);
    style->Colors[ImGuiCol_SeparatorActive]      = ImColor(255, 255, 255, 255);
    style->Colors[ImGuiCol_ResizeGrip]           = ImColor(60, 120, 200, 160);
    style->Colors[ImGuiCol_ResizeGripHovered]    = ImColor(80, 150, 255, 255);
    style->Colors[ImGuiCol_ResizeGripActive]     = ImColor(90, 160, 255, 255);
    style->Colors[ImGuiCol_Tab]                  = ImColor(30, 80, 150, 220);
    style->Colors[ImGuiCol_TabHovered]           = ImColor(50, 120, 200, 255);
    style->Colors[ImGuiCol_TabActive]            = ImColor(60, 130, 220, 255);
    style->Colors[ImGuiCol_TabUnfocused]         = ImColor(30, 60, 100, 150);
    style->Colors[ImGuiCol_TabUnfocusedActive]   = ImColor(40, 90, 160, 200);
}

uintptr_t il2cpp_base = 0;
void *getRealAddr(ulong offset) {
    return reinterpret_cast<void*>(il2cpp_base + offset);
}

bool clearMousePos = true;

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);
    if (!g_IsSetup) {
        SetupImgui();
        g_IsSetup = true;
    }
    ImGuiIO &io = ImGui::GetIO();
    SetElegantBlackAndBlueTheme();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();
    
    if (Class_Input__get_touchCount && Class_Input__GetTouch) {
        int touchCount = reinterpret_cast<int(*)()>(Class_Input__get_touchCount)();
        if (touchCount > 0) {
            UnityEngine_Touch_Fields touch = reinterpret_cast<UnityEngine_Touch_Fields(*)(int)>(Class_Input__GetTouch)(0);
            float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;
            
            switch ((TouchPhase)touch.m_Phase) {
                case TouchPhase::Began:
                case TouchPhase::Stationary:
                    io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                    io.MouseDown[0] = true;
                    break;
                case TouchPhase::Ended:
                case TouchPhase::Canceled:
                    io.MouseDown[0] = false;
                    clearMousePos = true;
                    break;
                case TouchPhase::Moved:
                    io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                    break;
                default:
                    break;
            }
        } else {
            io.MouseDown[0] = false;
        }
    }
    
    DrawESP((float)g_GlWidth, (float)g_GlHeight);
    ImGui::SetNextWindowSize(ImVec2((float)g_GlWidth * 0.34f, (float)g_GlHeight * 0.54f), ImGuiCond_Once);
    if (ImGui::Begin("MEOK CHEATS ZYGISK", 0, ImGuiWindowFlags_NoBringToFrontOnFocus)) {
        if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_FittingPolicyScroll)) {
            if (ImGui::BeginTabItem(OBFUSCATE("AIM"))) {
                ImGui::Checkbox(OBFUSCATE("ENABLE AIMBOT"), &Aimbot);
                ImGui::SliderFloat(OBFUSCATE("FOV"), &Fov_Aim, 0.0f, 500.0f, "%.f");
                ImGui::SliderFloat(OBFUSCATE("DISTANCE"), &Aimdis, 0.0f, 1000.0f, "%.f");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(OBFUSCATE("ESP"))) {
                ImGui::Checkbox("ENABLE ESP", &Enable);
                ImGui::Checkbox("ESP LINE", &Config.ESP.Line);
                ImGui::Checkbox("ESP BOX", &Config.ESP.Box);
                ImGui::Checkbox("ESP HEALTH", &Config.ESP.Health);
                ImGui::Checkbox("ESP INFO", &Config.ESP.RamaMods);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(OBFUSCATE("EXTRA"))) {
                ImGui::Checkbox("AIM SILENT", &SilentAim);
                ImGui::Checkbox(OBFUSCATE("CAMERA HACK"), &camxa);
                ImGui::Checkbox("RESET GUEST", &Guest);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(OBFUSCATE("DEV"))) {
                ImGui::Text("</> DEV: MEOK");
                ImGui::Text("TG OWNER: @MEOK");
                ImGui::Separator();
                ImGui::Text("DEVELOPER MEOK");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

void StartGUI() {
    void *ptr_eglSwapBuffer = DobbySymbolResolver("/system/lib64/libEGL.so", "eglSwapBuffers");
    if (!ptr_eglSwapBuffer) ptr_eglSwapBuffer = DobbySymbolResolver("/system/lib/libEGL.so", "eglSwapBuffers");
    if (ptr_eglSwapBuffer != nullptr) {
        DobbyHook(ptr_eglSwapBuffer, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
        LOGD("GUI Hooked");
    }
}

void hack_thread(pid_t pid) {
    for (int i = 0; i < 20; i++) {
        il2cpp_base = get_module_base(pid, "libil2cpp.so");
        if (il2cpp_base != 0) break;
        sleep(2);
    }
    if (il2cpp_base == 0) return;
    sleep(10);
    Il2CppAttach();
    
    DobbyHook((void*)Il2CppGetMethodOffset("Assembly-CSharp.dll", "COW.GamePlay", "Player", "UpdateRotation", 2), (void *) Modify_Bypass_RamaModz, (void **)&ori_bypass_ramamodz);
    DobbyHook((void*)Il2CppGetMethodOffset("Assembly-CSharp.dll", "COW.GamePlay", "FollowCamera", "get_OffsetForNormal", 0), (void *)_GetCameraHeightRateValue, (void **)&GetCameraHeightRateValue);
    DobbyHook((void*)Il2CppGetMethodOffset("Assembly-CSharp.dll", "COW", "GameConfig", "get_ResetGuest", 0), (void *)_ResetGuest, (void **)&ResetGuest);
    
    StartGUI();
}

void hack() {
    hack_thread(getpid());
}

REGISTER_ZYGISK_MODULE(MyModule)
