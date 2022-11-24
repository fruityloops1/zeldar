#pragma once

#include "types.h"

#include "nn/fs.h"

struct FlatBufferReadResult {
    u64 unk1;
    u64 unk2;
};

struct FlatBufferReadInfo {
    long mSize;
    long mPosition;
    nn::fs::FileHandle mHandle;
};

struct FlatBufferLoaderInfo
{
  char gap_0[56];
  const char *mPath;
};

struct FlatBufferLoader
{
  void *__vftable;
  void* field_8;
  char gap_10[48];
  FlatBufferLoaderInfo *mFileInfo;
  char gap_48[88];
  int field_A0;
  int field_A4;
  bool field_A8;
  FlatBufferReadInfo mReadInfo;
  char gap_C8[128];
  void* field_148;
  void* field_150;
  void* field_158;
  char gap_160[32];
  int field_180;
  void* struct_188;
  void* field_190;
  char gap_188[70];
  long field_1E0;
  void *mFlatBuffer;
  void* class_1F0;
  void* field_1F8;
  void* field_200;
};