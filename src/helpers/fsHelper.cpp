#include "diag/assert.hpp"
#include "fsHelper.h"
#include "nn/fs.h"
#include "nn/init.h"
#include "pe/DbgGui/DbgGui.h"

namespace FsHelper {
    nn::Result writeFileToPath(void* buf, size_t size, const char* path) {
        nn::fs::FileHandle handle;

        if (isFileExist(path)) {
            nn::fs::DeleteFile(path); // remove previous file
        }

        nn::Result result = nn::fs::CreateFile(path, size);

        if (result.isFailure()) {
            return result;
        }

        result = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Write);
        if (result.isFailure()) {
            return result;
        }

        result =
            nn::fs::WriteFile(handle, 0, buf, size, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
        if (result.isFailure()) {
            return result;
        }

        nn::fs::CloseFile(handle);

        return nn::Result(1);
    }

    // make sure to free buffer after usage is done
    void loadFileFromPath(LoadData& loadData) {

        nn::fs::FileHandle handle;

        EXL_ASSERT(FsHelper::isFileExist(loadData.path), "Failed to Find File!\nPath: %s", loadData.path);

        R_ABORT_UNLESS(nn::fs::OpenFile(&handle, loadData.path, nn::fs::OpenMode_Read));

        long size = 0;
        nn::fs::GetFileSize(&size, handle);
        loadData.buffer = pe::gui::getDbgGuiHeap()->tryAlloc(size, 4);
        loadData.bufSize = size;

        EXL_ASSERT(loadData.buffer, "Failed to Allocate Buffer! File Size: %ld", size);

        R_ABORT_UNLESS(nn::fs::ReadFile(handle, 0, loadData.buffer, size));

        nn::fs::CloseFile(handle);
    }

    long getFileSize(const char* path) {
        nn::fs::FileHandle handle;
        long result = -1;

        nn::Result openResult = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode::OpenMode_Read);

        if (openResult.isSuccess()) {
            nn::fs::GetFileSize(&result, handle);
            nn::fs::CloseFile(handle);
        }

        return result;
    }

    bool isFileExist(const char* path) {
        nn::fs::DirectoryEntryType type;
        nn::fs::GetEntryType(&type, path);

        return type == nn::fs::DirectoryEntryType_File;
    }
} // namespace FsHelper