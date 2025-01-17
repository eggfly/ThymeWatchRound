
#include "esp_heap_caps.h"
#include "../common.h"
#include "Arduino.h"

#define TAG "Memory"

void print_memory_info()
{
    uint32_t totalSize = ESP.getHeapSize();
    uint32_t maxAlloc = ESP.getMaxAllocHeap();
    MY_LOG("Total heap size: %lu bytes, Max alloc size: %lu bytes", totalSize, maxAlloc);
    // 获取可用的RAM内存大小
    size_t free_size = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t total_size = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);

    // Serial.print("Free memory: ");
    // Serial.print(free_size);
    // Serial.print(" bytes, Total memory: ");
    // Serial.print(total_size);
    // Serial.println(" bytes");
    MY_LOG("Free memory: %u bytes, Total memory: %u bytes", free_size, total_size);
    if (psramFound())
    {
        uint32_t psramSize = ESP.getPsramSize();
        uint32_t psramFree = ESP.getFreePsram();
        uint32_t psramMaxAlloc = ESP.getMaxAllocPsram();
        MY_LOG("Total PSRAM size: %lu bytes, Max alloc size: %lu bytes, Free PSRAM: %lu bytes", psramSize, psramMaxAlloc, psramFree);
    }
    else
    {
        MY_LOG("No PSRAM found");
    }
}
