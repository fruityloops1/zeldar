#include "patches.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void stubSocketInit() {
    patch::CodePatcher p(0xB3BB0C);
    p.WriteInst(inst::Nop());
}

void runCodePatches() {
    // stubSocketInit();
}