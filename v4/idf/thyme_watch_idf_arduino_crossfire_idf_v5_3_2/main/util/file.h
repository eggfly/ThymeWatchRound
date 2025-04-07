#pragma once
#include <vector>
#include <string>
#include <SD_MMC.h>
#include "FS.h"

struct FileInfo
{
    String path;
    String name;
    bool isDirectory;
    size_t size;
};

std::vector<FileInfo> listDir(const String &path);
