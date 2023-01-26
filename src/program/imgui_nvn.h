#pragma once

#include "types.h"
#include <imgui.h>

namespace nvnImGui {

    typedef void (*ProcDrawFunc)();

    struct BackendFuncPtrs {
        ImGuiMemAllocFunc imGuiMemAlloc;
        ImGuiMemFreeFunc imGuiMemFree;
    };

    void InstallHooks();

    bool InitImGui();

    void procDraw();

    void getAllocatorFuncs();

    void addDrawFunc(ProcDrawFunc func);
}
