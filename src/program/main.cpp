#include "lib.hpp"
#include <diag/debug.hpp>

#include "nn/err.h"
#include "nn/fs.h"

#include "helpers.h"
#include "imgui_nvn.h"
#include "pe/DbgGui/DbgGui.h"
#include <sead/heap/seadHeapMgr.h>

HOOK_DEFINE_TRAMPOLINE(MainInitHook) { static void Callback(); };

void MainInitHook::Callback() {
    Orig();

    R_ABORT_UNLESS(nn::fs::MountSdCardForDebug("sd").value);
}

void drawDbgGui() {
    sead::ScopedCurrentHeapSetter heapSetter(pe::gui::getDbgGuiHeap());

    auto* dbgGui = pe::gui::DbgGui::instance();
    if (dbgGui)
        dbgGui->draw();
}

// LTO moment
sead::ExpHeap* createExpHeap(size_t size, const char** name, sead::Heap* parent, s32 alignment,
                             sead::ExpHeap::HeapDirection direction, bool);

void createRootHeap() {
    const char* name = "DbgGuiHeap";
    pe::gui::getDbgGuiHeap() = createExpHeap(1024 * 1024 * 10, &name, sead::HeapMgr::sRootHeaps[0], 8,
                                             sead::Heap::cHeapDirection_Forward, false);

    pe::gui::DbgGui::createInstance(pe::gui::getDbgGuiHeap());
}

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();

    using Patcher = exl::patch::CodePatcher;
    using namespace exl::patch::inst;

    MainInitHook::InstallAtSymbol("nnMain");

    Patcher(0x00ebb0ec).BranchInst((void*)createRootHeap);

    // imgui hooks

    nvnImGui::InstallHooks();
    nvnImGui::addDrawFunc(drawDbgGui);
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}