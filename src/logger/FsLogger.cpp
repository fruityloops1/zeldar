#include "FsLogger.hpp"
#include "util.h"
#include "lib.hpp"

void FsLogger::log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buffer[0x500] = {};

    int count = nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args);
    if(count > 0) {
        nn::fs::WriteFile(instance().mHandle, instance().mOffset, buffer, count, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
    }

    va_end(args);
}

FsLogger& FsLogger::instance() {
    static FsLogger logger;
    return logger;
}

FsLogger::FsLogger() : mHandle() {
    nn::fs::CreateFile("sd:/amongesu.txt", 0x400000);
    R_ABORT_UNLESS(nn::fs::OpenFile(&mHandle, "sd:/amongesu.txt", nn::fs::OpenMode_Write));
}