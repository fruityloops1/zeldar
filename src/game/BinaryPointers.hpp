#pragma once

#include "nn/result.h"

/// @brief Allows for functions without symbols and static variables to be used/called by our code.
class BinaryPointers {
public:
    static BinaryPointers& instance();
    static void initValues();

    nn::Result (*socketInit)();
    
    bool *isDebugModePtr;

private:
    static constexpr uintptr_t socketInitOffset = 0x1698800;
    static constexpr uintptr_t isDebugModePtrOffset = 0x41DC078;
};