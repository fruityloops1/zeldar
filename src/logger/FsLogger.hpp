#pragma once

#include "nn/fs.h"

struct FsLogger {
    static void log(const char* fmt, ...);
private:
    static FsLogger& instance();
    FsLogger();

    nn::fs::FileHandle mHandle;
    int mOffset = 0;
};