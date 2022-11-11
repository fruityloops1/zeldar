#include "patches.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

// example (from SMO) of how to use codepatcher for specific asm edits
// void stubSocketInit() {
//     patch::CodePatcher p(0x95C498);
//     p.WriteInst(inst::Nop());
// }

void runCodePatches() {
    //stubSocketInit();
}