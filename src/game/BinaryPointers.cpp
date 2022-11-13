#include "BinaryPointers.hpp"
#include "lib.hpp"

BinaryPointers& BinaryPointers::instance() {
    static BinaryPointers instance;
    return instance;
}

void BinaryPointers::initValues() { 
    instance().socketInit = reinterpret_cast<nn::Result (*)()>(exl::util::modules::GetTargetOffset(socketInitOffset));
    instance().isDebugModePtr = reinterpret_cast<bool*>(exl::util::modules::GetTargetOffset(isDebugModePtrOffset));
}