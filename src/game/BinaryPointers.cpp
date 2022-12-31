#include "BinaryPointers.hpp"
#include "lib.hpp"
#include "nn/oe.h"
#include "logger/Logger.hpp"

FuncPtrTable pointers[] = {
    {
        .tableVer = "1.0.0",
        .readFileToBufferPtr = 0xBEC958,
        .flatBufferLoader = 0xBEBCD4,
        .flatBufferCreateFlatBuffer = 0xBEBB90,
        .flatBufferCreateFlatBuffer2 = 0x1DD78A4,
        .flatBufferCreateFlatBuffer3 = 0x1E211B0
    },
    {
        .tableVer = "1.0.1",
        .readFileToBufferPtr = 0xC2F8B8,
        .flatBufferLoader = 0xC2EC30,
        .flatBufferCreateFlatBuffer = 0xC2EAEC,
        .flatBufferCreateFlatBuffer2 = 0x1E27E00,
        .flatBufferCreateFlatBuffer3 = 0x1E72770
    },
    {
        .tableVer = "1.1.0",
        .readFileToBufferPtr = 0xC50058,
        .flatBufferLoader = 0xC4F3A8,
        .flatBufferCreateFlatBuffer = 0xC4F264,
        .flatBufferCreateFlatBuffer2 = 0x1E9E1E4,
        .flatBufferCreateFlatBuffer3 = 0x1EEB2E4
    }
};

BinaryPointers& BinaryPointers::instance() {
    static BinaryPointers instance;
    return instance;
}

FuncPtrTable *BinaryPointers::getVerPtrs() {
    
    nn::oe::DisplayVersion display_version;
    nn::oe::GetDisplayVersion(&display_version);

    for (size_t i = 0; i < ACNT(pointers); i++)
    {
        auto curPtrTbl = &pointers[i];

        if(strcmp(curPtrTbl->tableVer, display_version.name) == 0) {
            return curPtrTbl;
        }
    }

    EXL_ABORT(0, "Unable to find Offsets for Version: %s!", display_version.name);

    return nullptr;
}

void BinaryPointers::initValues() {

    FuncPtrTable *ptrs = getVerPtrs();

    BinaryPointers &inst = instance();

    inst.mPtrVer = ptrs->tableVer;

    inst.readFileToBuffer = reinterpret_cast<FlatBufferReadResult (*)(FlatBufferReadInfo* , void* , ulong)>(exl::util::modules::GetTargetOffset(ptrs->readFileToBufferPtr));

    R_ABORT_UNLESS(nn::ro::LookupSymbol(&ptrs->mallocPtr, "malloc"))
    R_ABORT_UNLESS(nn::ro::LookupSymbol(&ptrs->freePtr, "free"))
    R_ABORT_UNLESS(nn::ro::LookupSymbol(&ptrs->reallocPtr, "realloc"))

    inst.malloc = reinterpret_cast<void *(*)(size_t)>(ptrs->mallocPtr);
    inst.free = reinterpret_cast<void (*)(void*)>(ptrs->freePtr);
    inst.realloc = reinterpret_cast<void *(*)(void*, size_t)>(ptrs->freePtr);

}