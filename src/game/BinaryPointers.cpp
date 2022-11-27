#include "BinaryPointers.hpp"
#include "lib.hpp"
#include "nn/oe.h"
#include "logger/Logger.hpp"

struct FuncTableEntry {
    const char *mVersion;
    FuncPtrTable mTable;
};

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

    instance().mPtrVer = ptrs->tableVer;

    instance().readFileToBuffer = reinterpret_cast<FlatBufferReadResult (*)(FlatBufferReadInfo* , void* , ulong)>(exl::util::modules::GetTargetOffset(ptrs->readFileToBufferPtr));
    
}