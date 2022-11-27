#include "lib.hpp"
#include "patches.hpp"

#include "nn/err.h"
#include "nn/fs.h"
#include "nn/diag.h"

#include "logger/Logger.hpp"
#include "socket.h"
#include "game/BinaryPointers.hpp"
#include "game/ResourceLoading/ArcResource.hpp"

#include "helpers.hpp"

HOOK_DEFINE_TRAMPOLINE(MainInitHook) {
    static void Callback() {

        R_ABORT_UNLESS(nn::fs::MountSdCardForDebug("sd"));

        R_ABORT_UNLESS(Logger::instance().init(LOGGER_IP, 3080).value);

        Logger::log("Offsets loaded for Hooks: %s\n", BinaryPointers::getPtrsVer());

        Orig();

    }
};

HOOK_DEFINE_REPLACE(AbortImplHook) {
    static void Callback(const char *str1, char const *str2, const char *str3, s32 value) {
        Logger::log("Game Aborted!\n");

        // these strings will almost always be empty as the compiler tends to strip these for release builds
        Logger::log("String 1: %s\n", str1);
        Logger::log("String 2: %s\n", str2);
        Logger::log("String 3: %s\n", str3);
        Logger::log("Value: %d\n", value);

        exl::diag::DebugBreak();

        EXL_ABORT(value);
    }
};

HOOK_DEFINE_REPLACE(AbortImplOverloadHook) {
    static void Callback(const char *str1, char const *str2, const char *str3, s32 value, nn::Result result, const char *fmt, ...) {
        Logger::log("Game Aborted Using Overload!\n");

        // these strings will almost always be empty as the compiler tends to strip these for release builds
        Logger::log("String 1: %s\n", str1);
        Logger::log("String 2: %s\n", str2);
        Logger::log("String 3: %s\n", str3);
        Logger::log("Value: %d\n", value);

        va_list args;
        va_start(args, fmt);
        Logger::log(fmt, args);
        va_end(args);

        exl::diag::DebugBreak();

        EXL_ABORT(value);
    }
};

FlatBufferReadInfo newReadInfo;
nn::Result readResult = -1;

void NOINLINE fileRedirectionFunc(FlatBufferLoader *loader) {
    if(loader->mFileInfo && loader->field_148 && loader->field_A8) {

        readResult = nn::fs::OpenFile(&newReadInfo.mHandle, loader->mFileInfo->mPath, nn::fs::OpenMode::OpenMode_Read);

        if(readResult.isSuccess()) {

            Logger::log("Found Replacement file in romfs!\n");

            Logger::log("Path: %s\n", loader->mFileInfo->mPath);

            nn::fs::GetFileSize(&newReadInfo.mSize, newReadInfo.mHandle);

            Logger::log("Original Size: %lu New Size: %lu\n", loader->mBufferSize, newReadInfo.mSize);

            loader->mBufferSize = newReadInfo.mSize;
        }
    }
}

HOOK_DEFINE_TRAMPOLINE(FlatBufferCreateFlatBufferHook) {
    static FlatBufferReadResult Callback(FlatBufferLoader *thisPtr) {
        fileRedirectionFunc(thisPtr);
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(FlatBufferCreateFlatBuffer2Hook) {
    static FlatBufferReadResult Callback(FlatBufferLoader *thisPtr) {
        fileRedirectionFunc(thisPtr);
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(FlatBufferCreateFlatBuffer3Hook) {
    static FlatBufferReadResult Callback(FlatBufferLoader *thisPtr) {
        fileRedirectionFunc(thisPtr);
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(FlatBufferLoaderHook) {
    static FlatBufferReadResult Callback(FlatBufferLoader *thisPtr, void *buffer, ulong bufferSize) {
        
        if(thisPtr->mFileInfo && thisPtr->field_148 && thisPtr->field_A8) {
            
            if(readResult.isSuccess()) {

                newReadInfo.mPosition = 0;

                auto result = BinaryPointers::instance().readFileToBuffer(&newReadInfo, buffer, bufferSize);

                thisPtr->field_180 = 1;
                thisPtr->mReadPosition = newReadInfo.mPosition;

                return result;
            }
        }

        return Orig(thisPtr, buffer, bufferSize);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    // sets up function pointers needed to call funcs from the game binary
    BinaryPointers::initValues();

    MainInitHook::InstallAtSymbol("nnMain");

    FuncPtrTable *funcOffsets = BinaryPointers::getVerPtrs();

    // file system redirection

    FlatBufferLoaderHook::InstallAtOffset(funcOffsets->flatBufferLoader);

    FlatBufferCreateFlatBufferHook::InstallAtOffset(funcOffsets->flatBufferCreateFlatBuffer);

    FlatBufferCreateFlatBuffer2Hook::InstallAtOffset(funcOffsets->flatBufferCreateFlatBuffer2);

    FlatBufferCreateFlatBuffer3Hook::InstallAtOffset(funcOffsets->flatBufferCreateFlatBuffer3);

    runCodePatches();

}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}