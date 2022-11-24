#pragma once

#include "nn/result.h"
#include "game/ResourceLoading/FlatBufferLoader.hpp"

/// @brief Allows for functions without symbols and static variables to be used/called by our code.
class BinaryPointers {
public:
    static BinaryPointers& instance();
    static void initValues();

    nn::Result (*socketInit)();
    FlatBufferReadResult (*readFileToBuffer)(FlatBufferReadInfo* readInfo, void* buffer, ulong bufferSize);

    bool *isDebugModePtr;

private:

    static constexpr uintptr_t socketInitOffset = 0x1698800;
    static constexpr uintptr_t isDebugModePtrOffset = 0x41DC078;
    static constexpr uintptr_t readFileToBufferPtr = 0xBEC958;
};