/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This example uses SDMMC peripheral to communicate with SD card.

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_timer.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sd_test_io.h"
#if SOC_SDMMC_IO_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif

#define EXAMPLE_MAX_CHAR_SIZE 64

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"

#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
const char *names[] = {"CLK", "CMD", "D0", "D1", "D2", "D3"};
const int pins[] = {CONFIG_EXAMPLE_PIN_CLK,
                    CONFIG_EXAMPLE_PIN_CMD,
                    CONFIG_EXAMPLE_PIN_D0
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
                    ,
                    CONFIG_EXAMPLE_PIN_D1,
                    CONFIG_EXAMPLE_PIN_D2,
                    CONFIG_EXAMPLE_PIN_D3
#endif
};

const int pin_count = sizeof(pins) / sizeof(pins[0]);

#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
const int adc_channels[] = {CONFIG_EXAMPLE_ADC_PIN_CLK,
                            CONFIG_EXAMPLE_ADC_PIN_CMD,
                            CONFIG_EXAMPLE_ADC_PIN_D0
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
                            ,
                            CONFIG_EXAMPLE_ADC_PIN_D1,
                            CONFIG_EXAMPLE_ADC_PIN_D2,
                            CONFIG_EXAMPLE_ADC_PIN_D3
#endif
};
#endif // CONFIG_EXAMPLE_ENABLE_ADC_FEATURE

pin_configuration_t config = {
    .names = names,
    .pins = pins,
#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
    .adc_channels = adc_channels,
#endif
};
#endif // CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS

static esp_err_t s_example_write_file(const char *path, char *data)
{
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    return ESP_OK;
}

static esp_err_t s_example_read_file(const char *path)
{
    ESP_LOGI(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[EXAMPLE_MAX_CHAR_SIZE];
    fgets(line, sizeof(line), f);
    fclose(f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos)
    {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    return ESP_OK;
}

// BEGIN OF BENCHMARK CODE

// 按照需求配置总的测试文件大小（示例为 2MB）
#define TEST_FILE_SIZE (2 * 1024 * 1024)
// 每次写入/读取的块大小（4KB 仅作示例，可自行调整）
#define BLOCK_SIZE (4 * 1024)

// 用于写文件并测试写入速度的示例函数
static float test_write_speed_chunked(const char *filename)
{
    // 打开文件
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        ESP_LOGE(TAG, "Failed to open %s for write", filename);
        return 0.0f;
    }

    // 分配一块小缓存
    uint8_t *block = (uint8_t *)malloc(BLOCK_SIZE);
    if (!block)
    {
        ESP_LOGE(TAG, "Failed to malloc BLOCK_SIZE");
        fclose(f);
        return 0.0f;
    }

    // 准备测试数据（这里简单都写固定值，也可再灵活一点）
    memset(block, 0x55, BLOCK_SIZE);

    // 开始计时
    int64_t t_start = esp_timer_get_time();
    size_t written_total = 0;

    while (written_total < TEST_FILE_SIZE)
    {
        size_t to_write = BLOCK_SIZE;
        // 如果最后一段不足 BLOCK_SIZE，就写剩余的那一点
        if (written_total + BLOCK_SIZE > TEST_FILE_SIZE)
        {
            to_write = TEST_FILE_SIZE - written_total;
        }
        size_t w = fwrite(block, 1, to_write, f);
        if (w != to_write)
        {
            ESP_LOGE(TAG, "Write error or disk full");
            break;
        }
        written_total += w;
    }

    fclose(f);
    free(block);

    // 结束计时
    int64_t t_end = esp_timer_get_time();
    double time_s = (t_end - t_start) / 1000000.0; // 转换成秒

    // 计算写入速度（MB/s）
    float write_speed = (float)(TEST_FILE_SIZE / (1024.0 * 1024.0)) / time_s;
    return write_speed;
}

// 用于读文件并测试读取速度的示例函数
static float test_read_speed_chunked(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        ESP_LOGE(TAG, "Failed to open %s for read", filename);
        return 0.0f;
    }

    uint8_t *block = (uint8_t *)malloc(BLOCK_SIZE);
    if (!block)
    {
        ESP_LOGE(TAG, "Failed to malloc BLOCK_SIZE");
        fclose(f);
        return 0.0f;
    }

    int64_t t_start = esp_timer_get_time();
    size_t read_total = 0;

    while (read_total < TEST_FILE_SIZE)
    {
        size_t to_read = BLOCK_SIZE;
        if (read_total + BLOCK_SIZE > TEST_FILE_SIZE)
        {
            to_read = TEST_FILE_SIZE - read_total;
        }
        size_t r = fread(block, 1, to_read, f);
        if (r != to_read)
        {
            // 可能文件大小不够、读取出错等
            break;
        }
        read_total += r;
    }

    free(block);
    fclose(f);

    int64_t t_end = esp_timer_get_time();
    double time_s = (t_end - t_start) / 1000000.0;

    // 计算读取速度（MB/s）
    float read_speed = (float)(TEST_FILE_SIZE / (1024.0 * 1024.0)) / time_s;
    return read_speed;
}

// 在 app_main 下的适当位置（SD 卡挂载成功后）调用此测试
static void test_sdmmc_speed(void)
{
    const char *test_file = MOUNT_POINT "/test.bin";

    float w_speed = test_write_speed_chunked(test_file);
    if (w_speed > 0.0f)
    {
        ESP_LOGI(TAG, "Write speed: %.2f MB/s", w_speed);
    }

    float r_speed = test_read_speed_chunked(test_file);
    if (r_speed > 0.0f)
    {
        ESP_LOGI(TAG, "Read speed : %.2f MB/s", r_speed);
    }
}

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << 9, // 选择 GPIO9
        .mode = GPIO_MODE_OUTPUT,  // 配置为输出模式
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE // 中断模式关
    };
    gpio_config(&io_conf);

    // 拉高 GPIO9
    gpio_set_level(9, 1);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    esp_err_t ret;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.

    ESP_LOGI(TAG, "Using SDMMC peripheral");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz (range 400kHz - 40MHz for SDMMC)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = 50 * 1000;

    // For SoCs where the SD power can be supplied both via an internal or external (e.g. on-board LDO) power supply.
    // When using specific IO pins (which can be used for ultra high-speed SDMMC) to connect to the SD card
    // and the internal LDO power supply, we need to initialize the power supply first.
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_IO_ID,
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;

    ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create a new on-chip LDO power control driver");
        return;
    }
    host.pwr_ctrl_handle = pwr_ctrl_handle;
#endif

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // Set bus width to use:
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.width = 4;
#else
    slot_config.width = 1;
#endif

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
    slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
    slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
    slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
    slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
#endif // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
#endif // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
            check_sd_card_pins(&config, pin_count);
#endif
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    // Use POSIX and C standard library functions to work with files:

    // benchmark begin
    test_sdmmc_speed();
    // end

    // First create a file.
    const char *file_hello = MOUNT_POINT "/hello.txt";
    char data[EXAMPLE_MAX_CHAR_SIZE];
    snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
    ret = s_example_write_file(file_hello, data);
    if (ret != ESP_OK)
    {
        return;
    }

    const char *file_foo = MOUNT_POINT "/foo.txt";
    // Check if destination file exists before renaming
    struct stat st;
    if (stat(file_foo, &st) == 0)
    {
        // Delete it if it exists
        unlink(file_foo);
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
    if (rename(file_hello, file_foo) != 0)
    {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    ret = s_example_read_file(file_foo);
    if (ret != ESP_OK)
    {
        return;
    }

    // Format FATFS
#ifdef CONFIG_EXAMPLE_FORMAT_SD_CARD
    ret = esp_vfs_fat_sdcard_format(mount_point, card);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
        return;
    }

    if (stat(file_foo, &st) == 0)
    {
        ESP_LOGI(TAG, "file still exists");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "file doesn't exist, formatting done");
    }
#endif // CONFIG_EXAMPLE_FORMAT_SD_CARD

    const char *file_nihao = MOUNT_POINT "/nihao.txt";
    memset(data, 0, EXAMPLE_MAX_CHAR_SIZE);
    snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Nihao", card->cid.name);
    ret = s_example_write_file(file_nihao, data);
    if (ret != ESP_OK)
    {
        return;
    }

    // Open file for reading
    ret = s_example_read_file(file_nihao);
    if (ret != ESP_OK)
    {
        return;
    }

    // All done, unmount partition and disable SDMMC peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    // Deinitialize the power control driver if it was used
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    ret = sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to delete the on-chip LDO power control driver");
        return;
    }
#endif
}
