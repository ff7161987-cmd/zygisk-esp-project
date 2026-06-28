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
