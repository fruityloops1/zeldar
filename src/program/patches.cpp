#include "patches.hpp"
#include "logger/Logger.hpp"

#include "game/ResourceLoading/ArcResource.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void* (*origFunc)(ArcResource* thisPtr) = reinterpret_cast<void* (*)(ArcResource* thisPtr)>(exl::util::modules::GetTargetOffset(0xA2CE30));

void tempHook(ArcResource *thisPtr) {

    Logger::log("Unkown Ptr: %p\n", thisPtr->unkClass0);
    Logger::log("qword8: %x\n", thisPtr->qword8);
    Logger::log("qword10: %x\n", thisPtr->mPath);
    Logger::log("Path Length: %d\n", thisPtr->mPathLen);
    Logger::log("qword20: %x\n", thisPtr->qword20);

    exl::diag::DebugBreak();

    origFunc(thisPtr);
}

char* strchrHook(const char *str, char chr) {

    Logger::log("Str: %s\n", str);

    return strchr(str, chr);
}

void tempHookInstall() {
    // patch::CodePatcher p(0xA0C658);
    // p.BranchLinkInst((void*)tempHook);

    patch::CodePatcher p(0xA18078);
    p.BranchLinkInst((void*)strchrHook);

    // p.Seek(0xC96308);
    // p.BranchLinkInst((void*)tempHook);

    // p.Seek(0x11B4DD0);
    // p.WriteInst(inst::Nop());

}

void runCodePatches() {
    // tempHookInstall();
}