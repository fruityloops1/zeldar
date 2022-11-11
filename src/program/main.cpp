#include "lib.hpp"
#include "patches.hpp"
#include "nn/err.h"
#include "logger/Logger.hpp"
#include "socket.h"

HOOK_DEFINE_TRAMPOLINE(InitializeLogger) {
    static nn::Result Callback(nn::socket::Config const &config) {

        nn::Result result = Orig(config);

        R_ABORT_UNLESS(Logger::instance().init("10.0.0.224", 3080).value);

        return result;
    }
};


extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    InitializeLogger::InstallAtFuncPtr(nn::socket::Initialize);

    runCodePatches();

}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}