#include "imgui_nvn.h"
#include "game/BinaryPointers.hpp"
#include "helpers/InputHelper.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "lib.hpp"
#include "logger/Logger.hpp"
#include "nvn_CppFuncPtrImpl.h"

nvn::Device *nvnDevice;
nvn::Queue *nvnQueue;
nvn::CommandBuffer *nvnCmdBuf;

nvn::DeviceGetProcAddressFunc tempGetProcAddressFuncPtr;

nvn::CommandBufferInitializeFunc tempBufferInitFuncPtr;
nvn::DeviceInitializeFunc tempDeviceInitFuncPtr;
nvn::QueueInitializeFunc tempQueueInitFuncPtr;
nvn::QueuePresentTextureFunc tempPresentTexFunc;

nvn::CommandBufferSetViewportsFunc tempSetViewportFunc;

bool hasInitImGui = false;

void setViewport(nvn::CommandBuffer *cmdBuf, int first, int count, const float* ranges) {
    tempSetViewportFunc(cmdBuf, first, count, ranges);

    if (hasInitImGui) {
        auto &io = ImGui::GetIO();

        int x = ranges[0];
        int y = ranges[1];
        int w = ranges[2];
        int h = ranges[3];

        io.DisplaySize = ImVec2(w - x, h - y);

    }
}

void presentTexture(nvn::Queue *queue, nvn::Window *window, int texIndex) {

    if (hasInitImGui)
        nvnImGui::procDraw();

    tempPresentTexFunc(queue, window, texIndex);
}

NVNboolean deviceInit(nvn::Device *device, const nvn::DeviceBuilder *builder) {
    NVNboolean result = tempDeviceInitFuncPtr(device, builder);
    nvnDevice = device;
    nvn::nvnLoadCPPProcs(nvnDevice, tempGetProcAddressFuncPtr);
    return result;
}

NVNboolean queueInit(nvn::Queue *queue, const nvn::QueueBuilder *builder) {
    NVNboolean result = tempQueueInitFuncPtr(queue, builder);
    nvnQueue = queue;
    return result;
}

NVNboolean cmdBufInit(nvn::CommandBuffer *buffer, nvn::Device *device) {
    NVNboolean result = tempBufferInitFuncPtr(buffer, device);
    nvnCmdBuf = buffer;

    if (!hasInitImGui) {
        Logger::log("Initializing ImGui.\n");
        hasInitImGui = nvnImGui::InitImGui();
    }

    return result;
}

nvn::GenericFuncPtrFunc getProc(nvn::Device *device, const char *procName) {

    nvn::GenericFuncPtrFunc ptr = tempGetProcAddressFuncPtr(nvnDevice, procName);

    if (strcmp(procName, "nvnQueueInitialize") == 0) {
        tempQueueInitFuncPtr = (nvn::QueueInitializeFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &queueInit;
    } else if (strcmp(procName, "nvnCommandBufferInitialize") == 0) {
        tempBufferInitFuncPtr = (nvn::CommandBufferInitializeFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &cmdBufInit;
    } else if (strcmp(procName, "nvnCommandBufferSetViewports") == 0) {
        tempSetViewportFunc = (nvn::CommandBufferSetViewportsFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &setViewport;
    } else if (strcmp(procName, "nvnQueuePresentTexture") == 0) {
        tempPresentTexFunc = (nvn::QueuePresentTextureFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &presentTexture;
    }

    return ptr;
}

HOOK_DEFINE_TRAMPOLINE(DisableFullKeyState) {
    static int Callback(int *unkInt, nn::hid::NpadFullKeyState *state, int count, uint const &port) {

        int result = Orig(unkInt, state, count, port);

        if (!InputHelper::isReadInputs()) {
            if(InputHelper::isInputToggled())
                *state = nn::hid::NpadFullKeyState();
        }

        return result;
    }
};

HOOK_DEFINE_TRAMPOLINE(DisableHandheldState) {
    static int Callback(int *unkInt, nn::hid::NpadHandheldState *state, int count, uint const &port) {
        int result = Orig(unkInt, state, count, port);

        if (!InputHelper::isReadInputs()) {
            if(InputHelper::isInputToggled())
                *state = nn::hid::NpadHandheldState();
        }

        return result;
    }
};

HOOK_DEFINE_TRAMPOLINE(DisableJoyDualState) {
    static int Callback(int *unkInt, nn::hid::NpadJoyDualState *state, int count, uint const &port) {
        int result = Orig(unkInt, state, count, port);

        if (!InputHelper::isReadInputs()) {
            if(InputHelper::isInputToggled())
                *state = nn::hid::NpadJoyDualState();
        }

        return result;
    }
};

HOOK_DEFINE_TRAMPOLINE(DisableJoyLeftState) {
    static int Callback(int *unkInt, nn::hid::NpadJoyLeftState *state, int count, uint const &port) {
        int result = Orig(unkInt, state, count, port);

        if (!InputHelper::isReadInputs()) {
            if(InputHelper::isInputToggled())
                *state = nn::hid::NpadJoyLeftState();
        }

        return result;
    }
};

HOOK_DEFINE_TRAMPOLINE(DisableJoyRightState) {
    static int Callback(int *unkInt, nn::hid::NpadJoyRightState *state, int count, uint const &port) {
        int result = Orig(unkInt, state, count, port);

        if (!InputHelper::isReadInputs()) {
            if(InputHelper::isInputToggled())
                *state = nn::hid::NpadJoyRightState();
        }

        return result;
    }
};

HOOK_DEFINE_TRAMPOLINE(NvnBootstrapHook) {
    static void *Callback(const char *funcName) {

    void *result = Orig(funcName);

    Logger::log("Installing nvn Hooks.\n");

    if (strcmp(funcName, "nvnDeviceInitialize") == 0) {
        tempDeviceInitFuncPtr = (nvn::DeviceInitializeFunc) result;
        return (void *) &deviceInit;
    }
    if (strcmp(funcName, "nvnDeviceGetProcAddress") == 0) {
        tempGetProcAddressFuncPtr = (nvn::DeviceGetProcAddressFunc) result;
        return (void *) &getProc;
    }

    return result;
    }
};

void nvnImGui::procDraw() {

    ImguiNvnBackend::newFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    //    ImGui::ShowStyleSelector("Style Selector");
    //        ImGui::ShowMetricsWindow();
    //        ImGui::ShowDebugLogWindow();
    //        ImGui::ShowStackToolWindow();
    //        ImGui::ShowAboutWindow();
    //        ImGui::ShowFontSelector("Font Selector");
    //        ImGui::ShowUserGuide();

    ImGui::Render();
    ImguiNvnBackend::renderDrawData(ImGui::GetDrawData());
}

void nvnImGui::InstallHooks() {
    NvnBootstrapHook::InstallAtSymbol("nvnBootstrapLoader");

    DisableFullKeyState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadFullKeyStateEiRKj");
    DisableHandheldState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_17NpadHandheldStateEiRKj");
    DisableJoyDualState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadJoyDualStateEiRKj");
    DisableJoyLeftState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadJoyLeftStateEiRKj");
    DisableJoyRightState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_17NpadJoyRightStateEiRKj");
}

bool nvnImGui::InitImGui() {
    if (nvnDevice && nvnQueue && nvnCmdBuf) {

        Logger::log("Creating ImGui.\n");

        IMGUI_CHECKVERSION();

        ImGuiMemAllocFunc allocFunc = [](size_t size, void *user_data) {
            return BinaryPointers::instance().malloc(size);
        };

        ImGuiMemFreeFunc freeFunc = [](void *ptr, void *user_data) {
            BinaryPointers::instance().free(ptr);
        };

        Logger::log("Setting Allocator Functions.\n");

        ImGui::SetAllocatorFunctions(allocFunc, freeFunc, nullptr);

        Logger::log("Creating ImGui Context.\n");

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

        Logger::log("Setting Style to Dark.\n");

        ImGui::StyleColorsDark();

        ImguiNvnBackend::NvnBackendInitInfo initInfo = {
            .device = nvnDevice,
            .queue = nvnQueue,
            .cmdBuf = nvnCmdBuf
        };

        Logger::log("Initializing Backend.\n");

        ImguiNvnBackend::InitBackend(initInfo);

        InputHelper::initKBM();

        InputHelper::setPort(0); // set input helpers default port to zero

        return true;

    } else {
        Logger::log("Unable to create ImGui Renderer!\n");

        return false;
    }
}