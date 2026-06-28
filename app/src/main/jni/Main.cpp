#include "Struct/UniversalESP.h"
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

uintptr_t il2cpp_base = 0;
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
        "com.dts.freefiremax",
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

#include "Struct/RamaModz_Engine.h"

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
    ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
    ImGui::NewFrame();

    // Lógica de Toque Melhorada para Gráficos Suaves
    if (Class_Input__get_touchCount && Class_Input__GetTouch) {
        int touchCount = reinterpret_cast<int(*)()>(Class_Input__get_touchCount)();
        if (touchCount > 0) {
            UnityEngine_Touch_Fields touch = reinterpret_cast<UnityEngine_Touch_Fields(*)(int)>(Class_Input__GetTouch)(0);
            float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;
            io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
            
            // Persistência do estado de clique para evitar perda de input em baixo FPS
            if (touch.m_Phase == (int32_t)TouchPhase::Began || touch.m_Phase == (int32_t)TouchPhase::Moved || touch.m_Phase == (int32_t)TouchPhase::Stationary) {
                io.MouseDown[0] = true;
            } else {
                io.MouseDown[0] = false;
            }
        } else {
            io.MouseDown[0] = false;
        }
    }

    DrawESP((float)g_GlWidth, (float)g_GlHeight);

    ImGui::SetNextWindowSize(ImVec2((float)g_GlWidth * 0.4f, (float)g_GlHeight * 0.6f), ImGuiCond_Once);
    if (ImGui::Begin("ZYGISK SILENT | @RAMA_MODZ", 0)) {
        if (ImGui::BeginTabBar("Tabs")) {
            if (ImGui::BeginTabItem("AIM")) {
                ImGui::Checkbox("ENABLE AIMBOT", &Aimbot);
                ImGui::SliderFloat("FOV", &Fov_Aim, 0.0f, 500.0f, "%.f");
                ImGui::SliderFloat("DISTANCE", &Aimdis, 0.0f, 1000.0f, "%.f");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("ESP")) {
                ImGui::Checkbox("ENABLE ESP", &Enable);
                ImGui::Checkbox("ESP LINE", &Config.ESP.Line);
                ImGui::Checkbox("ESP BOX", &Config.ESP.Box);
                ImGui::Checkbox("ESP HEALTH", &Config.ESP.Health);
                ImGui::Checkbox("ESP INFO", &Config.ESP.RamaMods);
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

void SetElegantBlackAndBlueTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImLerp(style.Colors[ImGuiCol_Header], style.Colors[ImGuiCol_TitleBgActive], 0.80f);
    style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered];
    style.Colors[ImGuiCol_TabActive] = ImLerp(style.Colors[ImGuiCol_HeaderActive], style.Colors[ImGuiCol_TitleBgActive], 0.60f);
    style.Colors[ImGuiCol_TabUnfocused] = ImLerp(style.Colors[ImGuiCol_Tab], style.Colors[ImGuiCol_TitleBg], 0.80f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImLerp(style.Colors[ImGuiCol_TabActive], style.Colors[ImGuiCol_TitleBg], 0.40f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
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

    sleep(10); // Aumentado para garantir carregamento da Unity
    Il2CppAttach();
    
    UNIVERSAL_INIT("Assembly-CSharp.dll", "COW.GamePlay", "Match", "m_ShortIDToPlayers");

    void* addr;
    addr = (void*)Il2CppGetMethodOffset("Assembly-CSharp.dll", "COW.GamePlay", "Player", "UpdateRotation", 2);
    if (addr) DobbyHook(addr, (void *)Modify_Bypass_RamaModz, (void **)&ori_bypass_ramamodz);

    addr = (void*)Il2CppGetMethodOffset("Assembly-CSharp.dll", "COW.GamePlay", "FollowCamera", "get_OffsetForNormal", 0);
    if (addr) DobbyHook(addr, (void *)_GetCameraHeightRateValue, (void **)&GetCameraHeightRateValue);

    StartGUI();
}

void hack() {
    hack_thread(getpid());
}

REGISTER_ZYGISK_MODULE(MyModule)
