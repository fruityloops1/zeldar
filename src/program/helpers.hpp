#pragma once

#include "nn/fs.h"

bool isEqualString(const char16_t* pString_0, const char16_t* pString_1) {
    unsigned short val;

    while (1) {
        val = *pString_0;
        if (val != *pString_1) {
            break;
        }

        ++pString_1;
        ++pString_0;

        if (!val) {
            return true;
        }
    }

    return false;
}

bool isEqualString(const char* pString_0, const char* pString_1) {
    while (*pString_0 == *pString_1) {
        char val = *pString_0;

        if (!val)
            return true;

        ++pString_1;
        ++pString_0;
    }

    return false;
}

bool isEndWithString(const char* pString_0, const char* pString_1) {
    int pString0_Len = strlen(pString_0);
    int pString1_Len = strlen(pString_1);

    if (pString0_Len < pString1_Len)
        return false;

    return isEqualString(&pString_0[pString0_Len - pString1_Len], pString_1);
}

bool isEqualSubString(const char* pString_0, const char* pString_1) {
    return strstr(pString_0, pString_1);
}

long getFileSize(const char *path) {
    nn::fs::FileHandle handle;
    long result = -1;

    nn::Result openResult = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode::OpenMode_Read);

    if(openResult.isSuccess()) {
        nn::fs::GetFileSize(&result, handle);
        nn::fs::CloseFile(handle);
    }

    return result;
}

bool isFileExist(const char *path) {
    nn::fs::DirectoryEntryType type;
    nn::Result result = nn::fs::GetEntryType(&type, path);

    return type == nn::fs::DirectoryEntryType_File;
}