#pragma once

#include "nn/result.h"
#include "game/ResourceLoading/FlatBufferLoader.hpp"

struct FuncPtrTable {
    const char* tableVer;
    // general game ptrs

    // file loader pointers
    uintptr_t readFileToBufferPtr;
    // these seem to be members
    uintptr_t flatBufferLoader;
    uintptr_t flatBufferCreateFlatBuffer;
    uintptr_t flatBufferCreateFlatBuffer2;
    uintptr_t flatBufferCreateFlatBuffer3;
};

/// @brief Allows for functions without symbols and static variables to be used/called by our code.
class BinaryPointers {
public:
    static BinaryPointers& instance();
    static void initValues();
    static FuncPtrTable *getVerPtrs();
    static const char* getPtrsVer() { return instance().mPtrVer; }

    nn::Result (*socketInit)();
    FlatBufferReadResult (*readFileToBuffer)(FlatBufferReadInfo* readInfo, void* buffer, ulong bufferSize);
private:
    const char* mPtrVer;
};