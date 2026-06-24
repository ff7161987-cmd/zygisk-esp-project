#include <android/log.h>
#include <unistd.h>
#include <thread>
#include <vector>

#include <xdl.h>
#include <KittyUtils.h>
#include <KittyMemory.h>
#include <Il2Cpp.h>
#include "dobby.h"
#include "Struct/Gui.hpp"
#include "Struct/Drawing.h"
#include "Struct/Class.h"
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

bool esp_line = false;
bool esp_box = false;
bool esp_name = false;

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
        "com.dts.freefireth", "com.madfingergames.deadtrigger"
    };
    const char *process = env_->GetStringUTFChars(args->nice_name, nullptr);
    for (const auto *package: packages) {
      if (strcmp(process, package) == 0) {
        is_game_ = true;
        break;
      }
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
  Api *api_ = nullptr;
  JNIEnv *env_ = nullptr;
  bool is_game_ = false;
};

// ========================= ESP LOGIC ========================= \\

void DrawESP() {
    if (!esp_line && !esp_box && !esp_name) return;
    
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 screen_center = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y);
    ImVec2 dummy_pos = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    if (esp_line) Drawing::DrawLine(screen_center, dummy_pos, color);
    if (esp_box) Drawing::DrawBox(Rect(dummy_pos.x - 50, dummy_pos.y - 100, 100, 200), color);
    if (esp_name) Drawing::DrawText2(20.0f, ImVec2(dummy_pos.x - 20, dummy_pos.y - 120), color, "Enemy");
}

// ========================= IMGUI HOOK ========================= \\

uintptr_t il2cpp_base = 0;
inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);

    if (!g_IsSetup) {
        SetupImgui();
        g_IsSetup = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
    ImGui::NewFrame();

    DrawESP();

    ImGui::SetNextWindowSize(ImVec2((float) g_GlWidth * 0.30f, (float) g_GlHeight * 0.52f), ImGuiCond_Once);
    if (ImGui::Begin(OBFUSCATE("Zygisk Menu"), 0, ImGuiWindowFlags_NoBringToFrontOnFocus)) {
        ImGui::Checkbox("Line ESP", &esp_line);
        ImGui::Checkbox("Box ESP", &esp_box);
        ImGui::Checkbox("Name ESP", &esp_name);
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

void hack_thread(pid_t pid) {
    for (int i=0; i<10; i++) {
        il2cpp_base = get_module_base(pid, "libil2cpp.so");
        if (il2cpp_base != 0) break;
        sleep(5);
    }
    
    if (il2cpp_base != 0) {
        Il2CppAttach();
    }

    void *ptr_eglSwapBuffer = DobbySymbolResolver("/system/lib/libEGL.so", "eglSwapBuffers");
    if (ptr_eglSwapBuffer) {
        DobbyHook(ptr_eglSwapBuffer, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
    }
}

void hack() {
    std::thread thread_hack(hack_thread, getpid());
    thread_hack.detach();
}

REGISTER_ZYGISK_MODULE(MyModule)
