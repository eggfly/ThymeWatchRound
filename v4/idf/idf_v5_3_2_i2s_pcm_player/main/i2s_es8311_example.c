#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "PCM5102_PLAYER";

/*
 * 这两个符号由编译器在链接时注入，用来定位我们在 CMakeLists 中
 * 通过 EMBED_FILES 嵌入的 canon.pcm。
 */
extern const uint8_t _binary_canon_pcm_start[]; // PCM 文件开始位置
extern const uint8_t _binary_canon_pcm_end[];   // PCM 文件结束位置

/*
 * 根据实际硬件接线配置一下引脚
 * 注意 PCM5102 常见接线如下(仅供参考):
 *    BCK  -> GPIO 26
 *    LRC  -> GPIO 25
 *    DIN  -> GPIO 22
 * 如有 MCLK 需要，则再分配一个 MCLK 引脚
 */
#define I2S_BCK_IO 26
#define I2S_WS_IO 25
#define I2S_DO_IO 22
#define I2S_MCLK_IO -1 // 若需要 MCLK，可配置一个实际可用的 GPIO，若硬件无连接就保持 -1

// 假设 PCM 文件是 44.1kHz 采样，16 位立体声
#define SAMPLE_RATE 16000
#define BITS_PER_SAMPLE 16

static void i2s_init(void)
{
    // I2S 外设配置
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX, // 主机模式 + 只发数据
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = BITS_PER_SAMPLE,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // 立体声
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,      // 中断分配标志，默认为 0
        .dma_buf_count = 4,         // DMA 缓冲区数量
        .dma_buf_len = 1024,        // 每个缓冲区的大小(帧数量)
        .use_apll = false,          // 是否使用 APLL
        .tx_desc_auto_clear = true, // 启用自动清除
        .fixed_mclk = 0};

    // I2S 引脚配置
    i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_MCLK_IO,
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = -1 // 不接收
    };

    // 安装并启动 I2S
    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM_0, &pin_config));

    // 若需要设置更详细的时序或采样率，可以使用 i2s_set_clk 再行设置
    ESP_ERROR_CHECK(i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, BITS_PER_SAMPLE, I2S_CHANNEL_STEREO));
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing I2S...");
    i2s_init();

    // 拿到音频数据的起始和结束地址
    const uint8_t *audio_start = _binary_canon_pcm_start;
    const uint8_t *audio_end = _binary_canon_pcm_end;
    size_t file_size = audio_end - audio_start;
    ESP_LOGI(TAG, "PCM file size: %d bytes", file_size);

    // 当前读取位置
    size_t offset = 0;

    // 循环写数据到 I2S
    ESP_LOGI(TAG, "Start playing...");

    while (offset < file_size)
    {
        size_t to_write = 0;
        size_t bytes_written = 0;

        // 这里一次写 1024 或更大都可以，但要注意剩余大小
        to_write = (file_size - offset) > 1024 ? 1024 : (file_size - offset);

        // 写到 I2S DMA
        i2s_write(I2S_NUM_0, (const char *)(audio_start + offset), to_write, &bytes_written, portMAX_DELAY);

        offset += bytes_written;

        // 若要节流或避免在循环中占用全部 CPU，可加延时
        // vTaskDelay(pdMS_TO_TICKS(1));
    }

    ESP_LOGI(TAG, "Playback complete. Filling with zeros for a moment...");

    // 若想在播放完后清空缓冲(避免残留噪声)，可多写几次 0
    uint8_t zero_buf[512];
    memset(zero_buf, 0, sizeof(zero_buf));
    for (int i = 0; i < 10; i++)
    {
        size_t bytes_written = 0;
        i2s_write(I2S_NUM_0, zero_buf, sizeof(zero_buf), &bytes_written, portMAX_DELAY);
    }

    ESP_LOGI(TAG, "Done. Stopping I2S...");

    // 停止 i2s 并卸载驱动
    i2s_driver_uninstall(I2S_NUM_0);
    ESP_LOGI(TAG, "I2S uninstalled. Task finished.");
}