
#include "../common.h"
#include "Arduino.h"
#include "file.h"
#include "config.h"

#define TAG "File"

// void populateMusicFileList(String path, size_t depth)
// {
//     Serial.printf("search: %s, depth=%d\n", path.c_str(), depth);
//     File musicDir = FS_INSTANCE.open(path);
//     bool nextFileFound;
//     do
//     {
//         nextFileFound = false;
//         File entry = musicDir.openNextFile();
//         if (entry)
//         {
//             nextFileFound = true;
//             if (entry.isDirectory())
//             {
//                 if (depth)
//                 {
//                     populateMusicFileList(entry.path(), depth - 1);
//                 }
//             }
//             else
//             {
//                 const bool entryIsFile = entry.size() > 4096;
//                 if (entryIsFile)
//                 {
//                     if (APP_DEBUG)
//                     {
//                         Serial.print(entry.path());
//                         Serial.print(" size=");
//                         Serial.println(entry.size());
//                     }
//                     if (endsWithIgnoreCase(entry.name(), ".mp3") || endsWithIgnoreCase(entry.name(), ".flac") || endsWithIgnoreCase(entry.name(), ".aac") || endsWithIgnoreCase(entry.name(), ".wav"))
//                     {
//                         m_songFiles.push_back(entry.path());
//                     }
//                 }
//             }
//             entry.close();
//         }
//     } while (nextFileFound);
// }

std::vector<FileInfo> listDir(const String &path)
{
    std::vector<FileInfo> fileList;
    File root = FS_INSTANCE.open(path);

    if (!root || !root.isDirectory())
    {
        if (root)
            root.close();
        return fileList;
    }

    File entry;
    while ((entry = root.openNextFile()))
    {
        FileInfo info;

        // 获取基础属性
        info.isDirectory = entry.isDirectory();
        info.size = entry.size();

        // 优化路径存储
        info.path = path;
        if (!path.endsWith("/"))
            info.path += "/";
        info.path += entry.name();

        // MY_LOG("  %s: %s  SIZE: %u, path=%s", entry.isDirectory() ? "DIR" : "FILE", entry.name(), entry.size(), entry.path());

        // 分离文件名
        const char *lastSlash = strrchr(entry.name(), '/');
        info.name = lastSlash ? lastSlash + 1 : entry.name();

        fileList.push_back(info);
        entry.close(); // 立即关闭释放资源

        // 内存保护：限制最大文件数
        if (fileList.size() >= MAX_FILE_LIST_SIZE)
        {
            break;
        }
    }

    root.close();
    return fileList;
}
