#include "lib.hpp"
#include "patches.hpp"
#include "nn/err.h"
#include "nn/fs.h"
#include "logger/Logger.hpp"
#include "socket.h"
#include "game/BinaryPointers.hpp"

HOOK_DEFINE_TRAMPOLINE(MainInitHook) {
    static void Callback() {

        R_ABORT_UNLESS(nn::fs::MountSdCardForDebug("sd"));

        R_ABORT_UNLESS(Logger::instance().init("10.0.0.224", 3080).value);

        Orig();

    }
};

// currently doesnt do anything, most likely never gets called as nothing needs to
HOOK_DEFINE_REPLACE(DebugPrintHook) {
    static void Callback(void* unk1, void *unk2, const char *fmt, ...) {

        va_list args;
        va_start(args, fmt);
        Logger::log(fmt, args);
        va_end(args);

    }
};

// hooks over the default nn::fs openfile, however it isnt exactly useful for anything inside the data archive.
HOOK_DEFINE_TRAMPOLINE(OpenFileHook) {
    static nn::Result Callback(nn::fs::FileHandle* handleOut, char const* path, int mode) {
        
        Logger::log("Opening File At: %s\n", path);

        return Orig(handleOut, path, mode);
    }
};

// unsure of the contents of this struct, size was purely a guess based loosely off of stack allocations
struct ArcDataStruct {
    char size[0x40];
};

// hooks over what i believe is what the game mainly uses for accessing files within the data archive 
HOOK_DEFINE_TRAMPOLINE(ArcLoadHook) {
    static ArcDataStruct Callback(const char **path) {
        Logger::log("Loading Arc File at: %s\n", path[0]);
        return Orig(path);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    BinaryPointers::initValues();

    MainInitHook::InstallAtOffset(0x1439F40);

    DebugPrintHook::InstallAtOffset(0x2D07104);

    OpenFileHook::InstallAtFuncPtr(nn::fs::OpenFile);

    ArcLoadHook::InstallAtOffset(0xC96254);

    runCodePatches();

}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}