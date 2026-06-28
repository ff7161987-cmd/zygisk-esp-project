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
    env->GetJavaVM(&jvm);
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
      hack();
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
    void *ptr_eglSwapBuffer = DobbySymbolResolver(NULL, "eglSwapBuffers");
    if (ptr_eglSwapBuffer != nullptr) {
        DobbyHook(ptr_eglSwapBuffer, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
        LOGD("GUI Hooked");
    }
}

void hack_thread(pid_t pid) {
    LOGD("Hack Thread initiated for PID: %i", pid);
    for (int i = 0; i < 20; i++) {
        il2cpp_base = get_module_base(pid, "libil2cpp.so");
        if (il2cpp_base != 0) break;
        sleep(5);
    }
    if (il2cpp_base == 0) {
        LOGE("libil2cpp.so not found");
        return;
    }
    
    sleep(10);
    Il2CppAttach();
    
    void* addr_bypass = (void*)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("UpdateRotation"), 2);
    if (addr_bypass) DobbyHook(addr_bypass, (void *) Modify_Bypass_RamaModz, (void **)&ori_bypass_ramamodz);

    void* addr_camera = (void*)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("FollowCamera"), OBFUSCATE("get_OffsetForNormal"), 0);
    if (addr_camera) DobbyHook(addr_camera, (void *)_GetCameraHeightRateValue, (void **)&GetCameraHeightRateValue);

    void* addr_guest = (void*)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), OBFUSCATE("GameConfig"), OBFUSCATE("get_ResetGuest"), 0);
    if (addr_guest) DobbyHook(addr_guest, (void *)_ResetGuest, (void **)&ResetGuest);
    
    StartGUI();
}

void hack() {
    LOGD("Inject Ok");
    std::thread thread_hack(hack_thread, getpid());
    thread_hack.detach();
}

REGISTER_ZYGISK_MODULE(MyModule)
