#include "ThymeWatchFace.h"
#include <common.h>
#include <config.h>
#include "hal/hal_ls012b7dd06.h"
#include "hal/hal_rtc.h"
#include <RasterGothic18CondBold18pt7b.h>

#include "hal/hal_ls012b7dd06.h"
#include "resources/casio_yellow.h"
#include "resources/lcd_digits.h"
#include "resources/anim_data_12_frames.h"

#include "util/ui.h"
#include "util/system.h"
#include "util/str.h"

#include <apps/apps.h>
#include "framework/app_manager.h"
#include "Audio.h" //https://github.com/schreibfaul1/ESP32-audioI2S

#define TAG "ThymeWatchFace"

// #define FONT u8g2_font_wqy12_t_gb2312b
// #define FONT u8g2_font_wqy14_t_gb2312b
// u8g2_font_wqy16_t_gb2312

#define FONT u8g2_font_chill7_h_cjk

using namespace Thyme;

std::vector<String> m_songFiles{};

char music_title[256] = "";
char music_album[256] = "";
char music_artist[256] = "";

void populateMusicFileList(String path, size_t depth)
{
    Serial.printf("search: %s, depth=%d\n", path.c_str(), depth);
    File musicDir = FS_INSTANCE.open(path);
    bool nextFileFound;
    do
    {
        nextFileFound = false;
        File entry = musicDir.openNextFile();
        if (entry)
        {
            nextFileFound = true;
            if (entry.isDirectory())
            {
                if (depth)
                {
                    populateMusicFileList(entry.path(), depth - 1);
                }
            }
            else
            {
                const bool entryIsFile = entry.size() > 4096;
                if (entryIsFile)
                {
                    if (APP_DEBUG)
                    {
                        Serial.print(entry.path());
                        Serial.print(" size=");
                        Serial.println(entry.size());
                    }
                    if (endsWithIgnoreCase(entry.name(), ".mp3") || endsWithIgnoreCase(entry.name(), ".flac") || endsWithIgnoreCase(entry.name(), ".aac") || endsWithIgnoreCase(entry.name(), ".wav"))
                    {
                        m_songFiles.push_back(entry.path());
                    }
                }
            }
            entry.close();
        }
    } while (nextFileFound);
}

ThymeWatchFace::ThymeWatchFace(void *params) : ThymeApp(params)
{
    mIsAdjust = (params != nullptr);
    MY_LOG("ThymeWatchFace::ThymeWatchFace(), isAdjust: %d", mIsAdjust);
}

std::string ThymeWatchFace::appId()
{
    return "watch_face";
}

Audio audio;

bool playerInitialized = false;

void startPlayOnce()
{
    if (!playerInitialized)
    {
        // bool started = audio.connecttoFS(SD_MMC, "/蔡琴 - 渡口.mp3"); // mp3
        // bool started = audio.connecttoFS(SD_MMC, "/1 - Hotel California.flac"); // flac
        bool started = audio.connecttoFS(SD_MMC, "/逃跑计划-阳光照进回忆里.mp3"); // mp3
        MY_LOG("connecttoFS(), started: %d", started);
        playerInitialized = true;
    }
}

void ThymeWatchFace::onStart(Arduino_Canvas_6bit *gfx)
{
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(7); // 0...21
    MY_LOG("ThymeWatchFace::onStart()");
}

void ThymeWatchFace::onStop(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("ThymeWatchFace::onStop()");
}

const char three_body_problem[] =
    "罗辑站在叶文洁面前，心中充满了疑惑和不安。他刚刚从叶文洁那里得知了宇宙社会学的核心理论，而这个理论将彻底改变他对宇宙的认知。\n"
    "叶文洁缓缓地说道：“罗辑，你知道宇宙中有多少文明吗？”\n"
    "罗辑摇了摇头：“我不知道，但根据德雷克方程，宇宙中的文明数量应该是非常庞大的。”\n"
    "叶文洁微微一笑：“是的，宇宙中的文明数量确实庞大，但你知道它们为什么没有出现在我们面前吗？”\n"
    "罗辑沉思片刻，回答道：“可能是因为距离太远，或者它们的技术还没有达到能够进行星际旅行的水平。”\n"
    "叶文洁摇了摇头：“不，罗辑，真正的原因远比这更残酷。宇宙中的每一个文明都是猎人，它们隐藏在黑暗中，小心翼翼地隐藏自己，同时也在寻找其他文明。一旦发现其他文明的存在，它们会毫不犹豫地将其消灭。”\n"
    "罗辑震惊地看着叶文洁：“这……这怎么可能？为什么它们要这么做？”\n"
    "叶文洁的目光变得深邃：“因为宇宙的资源是有限的，而文明的发展是无限的。任何一个文明的存在，都会对其他文明构成威胁。为了生存，它们必须消灭潜在的竞争对手。”\n"
    "罗辑感到一阵寒意：“那……那我们该怎么办？如果宇宙真的是这样一个黑暗的森林，我们岂不是随时都可能被其他文明消灭？”\n"
    "叶文洁点了点头：“是的，罗辑，这就是宇宙的真相。我们唯一能做的，就是隐藏自己，不要暴露在宇宙的黑暗中。只有这样，我们才能生存下去。”\n"
    "罗辑沉默了片刻，突然问道：“那……如果我们发现了其他文明，我们该怎么办？”\n"
    "叶文洁的目光变得冰冷：“消灭它，毫不犹豫地消灭它。因为如果你不消灭它，它就会消灭你。”\n"
    "罗辑感到一阵窒息，他从未想过宇宙竟然如此残酷。他低声问道：“这就是宇宙社会学的核心理论吗？”\n"
    "叶文洁点了点头：“是的，罗辑，这就是‘黑暗森林法则’。宇宙是一片黑暗的森林，每一个文明都是带枪的猎人，像幽灵般潜行于林间，轻轻拨开挡路的树枝，竭力不让脚步发出一点儿声音，连呼吸都必须小心翼翼。他必须小心，因为林中到处都有与他一样潜行的猎人。如果他发现了别的生命，能做的只有一件事：开枪消灭之。在这片森林中，他人就是地狱，就是永恒的威胁，任何暴露自己存在的生命都将很快被消灭。”\n"
    "罗辑感到一阵深深的无力感，他意识到自己正站在一个巨大的谜团面前，而这个谜团的答案将决定人类文明的命运。";

const char shen_nong_c_str[] =
    "Shen Nong\n\n"
    "神农一生下来就是\n"
    "个水晶肚子，五脏\n"
    "六腑全都能看得一\n"
    "清二楚。那时侯，\n"
    "人们经常因为乱吃\n"
    "东西而生病，甚至\n"
    "丧命。神农决心尝\n"
    "遍所有的东西，好\n"
    "吃的放在身边左边\n"
    "的袋子里，给人吃\n"
    "；\n"
    "不好吃的就放在身\n"
    "子右边的袋子里，\n"
    "作药用。\n"
    "第一次，神农尝了\n"
    "一片小嫩叶。这叶\n"
    "片一落进肚里，就\n"
    "上上下下地把里面\n"
    "各器官擦洗得清清\n"
    "爽爽，\n"
    "象巡查似的，\n"
    "神农把它叫做\n"
    "“查”，\n"
    "就是后人所称的\n"
    "“茶”。\n"
    "神农将它放进右边\n"
    "袋子里。第二次，\n"
    "神农尝了朵蝴蝶样\n"
    "的淡红小花，甜津\n"
    "津的，香味扑鼻，\n"
    "这是“甘草”。他把\n"
    "它放进了左边袋子\n"
    "里。就这样，神农\n"
    "辛苦地尝遍百草，\n"
    "每次中毒，都靠茶\n"
    "来解救。后来，\n"
    "他左边的袋子里花\n"
    "草根叶有四万七千\n"
    "种，右边有三十九\n"
    "万八千种。\n"
    "但有一天，神农尝\n"
    "到了“断肠草”，这\n"
    "种毒草太厉害了，\n"
    "他还来不及吃茶解\n"
    "毒就死了。\n"
    "他是为了拯救人们\n"
    "而牺牲的，人们称\n"
    "他为“药王菩萨”，\n"
    "人间以这个神话故\n"
    "事永远地纪念他。\n";

size_t getLineCharCount(const char *str)
{
    size_t count = 0;
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\n')
        {
            count++;
        }
    }
    return count;
}

size_t getNextLineWrapCharPos(const char *str, size_t startPos)
{
    size_t i = startPos;
    while (str[i] != '\n')
    {
        i++;
    }
    return i;
}

// START: weak functions
// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}

void audio_id3data(const char *info)
{
    // id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
    if (startsWithIgnoreCase("TITLE", info))
    {
        strncpy(music_title, info + sizeof("TITLE") + 1, sizeof(music_title) / sizeof(music_title[0]));
    }
    if (startsWithIgnoreCase("ALBUM", info))
    {
        strncpy(music_album, info + sizeof("ALBUM") + 1, sizeof(music_album) / sizeof(music_album[0]));
    }
    if (startsWithIgnoreCase("ARTIST", info))
    {
        strncpy(music_artist, info + sizeof("ARTIST") + 1, sizeof(music_artist) / sizeof(music_artist[0]));
    }
    // drawScreen();
}

void audio_eof_mp3(const char *info)
{
    // end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}

void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}

void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}

void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}

void audio_commercial(const char *info)
{
    // duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}

void audio_icyurl(const char *info)
{
    // homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}

void audio_lasthost(const char *info)
{
    // stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}

void audio_eof_speech(const char *info)
{
    Serial.print("eof_speech  ");
    Serial.println(info);
}
// END: weak functions

void ThymeWatchFace::onDraw(Arduino_Canvas_6bit *gfx)
{
    startPlayOnce();
    audio.loop();
    unsigned long currTime = millis();
    if (currTime - rtcUpdateTime > RTC_UPDATE_INTERVAL)
    {
        tm = readRTC();
        rtcUpdateTime = currTime;
    }

    if (currTime - sensorUpdateTime > SENSOR_UPDATE_INTERVAL)
    {
        // TODO
        uint32_t currAudioTime = audio.getAudioCurrentTime();
        MY_LOG("Audio current time: %lu", currAudioTime);
        pressure_t pressure_data = read_pressure();
        snprintf(buf, sizeof(buf), "\n T: %.3f C\n P: %.3f Pa\n A: %.3f m",
                 pressure_data.temperature,
                 pressure_data.pressure,
                 pressure_data.altitude);
        // tm = readRTC();
        // printDateTime(tm);
        va_meter_data = read_va_meter();
        print_va_meter(va_meter_data);
        // snprintf(va_buf, sizeof(va_buf), "Bus Voltage: %.3f V, Shunt Voltage: %.2f mV, Load Voltage: %.2f V, Current: %.2f mA, Power: %.2f mW", va_meter_data.busvoltage, va_meter_data.shuntvoltage, va_meter_data.loadvoltage, va_meter_data.current_mA, va_meter_data.power_mW);
        // snprintf(va_buf, sizeof(va_buf), "%.3fV/%.2fmA", va_meter_data.loadvoltage, va_meter_data.current_mA);
        // MY_LOG("%s", va_buf);
        read_fuel_gauge(&fuel_gauge_data);
        print_fuel_gauge(fuel_gauge_data);
        snprintf(power_buf, sizeof(power_buf), "%.2fV %.2fmA %.1f%%", fuel_gauge_data.cellVoltage, va_meter_data.current_mA, fuel_gauge_data.cellPercent);
        sensorUpdateTime = currTime;
    }

    if (currTime - imuUpdateTime > IMU_UPDATE_INTERVAL)
    {
        read_imu(&imu_data);
        if (imu_data.acc_valid && imu_data.gyro_valid)
        {
            MY_LOG("IMU: %f %f %f, %f %f %f", imu_data.acc_x, imu_data.acc_y, imu_data.acc_z, imu_data.gyro_x, imu_data.gyro_y, imu_data.gyro_z);
        }
        else
        {
            MY_LOG("IMU data not available");
        }
        imuUpdateTime = currTime;
    }

    if (currentWatchFaceIndex == 0)
    {
        // Start: renderCasioWatchFace
        memcpy(gfx->getFramebuffer(), casio_yellow, 240 * 240);
        // 小时的第1位
        gfx->drawBitmap(46, 114, digits_20x44[tm.Hour / 10], 20, 44, RGB565_BLACK);
        // 小时的第2位
        gfx->drawBitmap(71, 114, digits_20x44[tm.Hour % 10], 20, 44, RGB565_BLACK);
        // 分钟的第1位
        gfx->drawBitmap(107, 114, digits_20x44[tm.Minute / 10], 20, 44, RGB565_BLACK);
        // 分钟的第2位
        gfx->drawBitmap(132, 114, digits_20x44[tm.Minute % 10], 20, 44, RGB565_BLACK);
        // 秒钟的第1位
        gfx->drawBitmap(160, 129, digits_14x29[tm.Second / 10], 14, 29, RGB565_BLACK);
        // 秒钟的第2位
        gfx->drawBitmap(178, 129, digits_14x29[tm.Second % 10], 14, 29, RGB565_BLACK);
        printCenteredText(gfx, power_buf, COLOR_BLACK, 1, 120, 120);
        drawIMUPointer(gfx);
        flushDisplay(gfx->getFramebuffer());
    }
    else if (currentWatchFaceIndex == 1)
    {
        renderNyanCat(gfx);
        drawIMUPointer(gfx);
    }
    else if (currentWatchFaceIndex == 2)
    {
        time_t unix = makeTime(tm);
        unix += lastDigitalCrownPosition * 60;
        tmElements_t adjustedTime;
        breakTime(unix, adjustedTime);

        gfx->fillScreen(RGB565_BLACK);
        // Draw 12 hour digits
        std::string hours[12] = {"12", "1", "2", "3", "4", "5",
                                 "6", "7", "8", "9", "10", "11"};
        for (int i = 0; i < 12; i++)
        {
            int16_t x = 120 + 100 * cos(i * 30 * DEG_TO_RAD - HALF_PI);
            int16_t y = 120 + 100 * sin(i * 30 * DEG_TO_RAD - HALF_PI);
            printCenteredText(gfx, hours[i].c_str(), RGB565_WHITE, 2, x, y);
        }
        // hours, minutes, seconds pointer watch face
        // Hour
        int16_t hourThinkness = 4;
        int16_t hourRadius = 50;
        int16_t hourAngle = (adjustedTime.Hour % 12) * 30 + (adjustedTime.Minute / 2);
        int16_t hour_x0 = 120 - hourThinkness * cos(hourAngle * DEG_TO_RAD);
        int16_t hour_y0 = 120 - hourThinkness * sin(hourAngle * DEG_TO_RAD);
        int16_t hour_x1 = 120 + hourThinkness * cos(hourAngle * DEG_TO_RAD);
        int16_t hour_y1 = 120 + hourThinkness * sin(hourAngle * DEG_TO_RAD);
        int16_t hour_x = 120 + hourRadius * cos(hourAngle * DEG_TO_RAD - HALF_PI);
        int16_t hour_y = 120 + hourRadius * sin(hourAngle * DEG_TO_RAD - HALF_PI);
        int16_t hour_x2 = hour_x + hourThinkness * cos(hourAngle * DEG_TO_RAD);
        int16_t hour_y2 = hour_y + hourThinkness * sin(hourAngle * DEG_TO_RAD);
        int16_t hour_x3 = hour_x - hourThinkness * cos(hourAngle * DEG_TO_RAD);
        int16_t hour_y3 = hour_y - hourThinkness * sin(hourAngle * DEG_TO_RAD);
        gfx->fillTriangle(hour_x0, hour_y0, hour_x1, hour_y1, hour_x2, hour_y2, RGB565_WHITE);
        gfx->fillTriangle(hour_x2, hour_y2, hour_x3, hour_y3, hour_x0, hour_y0, RGB565_WHITE);

        // Minute
        int16_t minuteThicknes = 3;
        int16_t minuteRadius = 70;
        int16_t minuteAngle = adjustedTime.Minute * 6;
        int16_t minute_x0 = 120 - minuteThicknes * cos(minuteAngle * DEG_TO_RAD);
        int16_t minute_y0 = 120 - minuteThicknes * sin(minuteAngle * DEG_TO_RAD);
        int16_t minute_x1 = 120 + minuteThicknes * cos(minuteAngle * DEG_TO_RAD);
        int16_t minute_y1 = 120 + minuteThicknes * sin(minuteAngle * DEG_TO_RAD);
        int16_t minute_x = 120 + minuteRadius * cos(minuteAngle * DEG_TO_RAD - HALF_PI);
        int16_t minute_y = 120 + minuteRadius * sin(minuteAngle * DEG_TO_RAD - HALF_PI);
        int16_t minute_x2 = minute_x + minuteThicknes * cos(minuteAngle * DEG_TO_RAD);
        int16_t minute_y2 = minute_y + minuteThicknes * sin(minuteAngle * DEG_TO_RAD);
        int16_t minute_x3 = minute_x - minuteThicknes * cos(minuteAngle * DEG_TO_RAD);
        int16_t minute_y3 = minute_y - minuteThicknes * sin(minuteAngle * DEG_TO_RAD);
        gfx->fillTriangle(minute_x0, minute_y0, minute_x1, minute_y1, minute_x2, minute_y2, RGB565_GREEN);
        gfx->fillTriangle(minute_x2, minute_y2, minute_x3, minute_y3, minute_x0, minute_y0, RGB565_GREEN);

        // Seconds
        int16_t secondRadius = 90;
        int16_t secondAngle = adjustedTime.Second * 6;
        bool thinest = true;
        if (thinest)
        {
            int16_t second_x = 120 + secondRadius * cos(secondAngle * DEG_TO_RAD - HALF_PI);
            int16_t second_y = 120 + secondRadius * sin(secondAngle * DEG_TO_RAD - HALF_PI);
            gfx->drawLine(120, 120, second_x, second_y, RGB565_RED);
        }
        else
        {
            int16_t secondThicknes = 2;
            int16_t second_x0 = 120 - secondThicknes * cos(secondAngle * DEG_TO_RAD);
            int16_t second_y0 = 120 - secondThicknes * sin(secondAngle * DEG_TO_RAD);
            int16_t second_x1 = 120 + secondThicknes * cos(secondAngle * DEG_TO_RAD);
            int16_t second_y1 = 120 + secondThicknes * sin(secondAngle * DEG_TO_RAD);
            int16_t second_x = 120 + secondRadius * cos(secondAngle * DEG_TO_RAD - HALF_PI);
            int16_t second_y = 120 + secondRadius * sin(secondAngle * DEG_TO_RAD - HALF_PI);
            int16_t second_x2 = second_x + secondThicknes * cos(secondAngle * DEG_TO_RAD);
            int16_t second_y2 = second_y + secondThicknes * sin(secondAngle * DEG_TO_RAD);
            int16_t second_x3 = second_x - secondThicknes * cos(secondAngle * DEG_TO_RAD);
            int16_t second_y3 = second_y - secondThicknes * sin(secondAngle * DEG_TO_RAD);
            gfx->fillTriangle(second_x0, second_y0, second_x1, second_y1, second_x2, second_y2, RGB565_RED);
            gfx->fillTriangle(second_x2, second_y2, second_x3, second_y3, second_x0, second_y0, RGB565_RED);
        }
        gfx->fillCircle(120, 120, hourThinkness, RGB565_RED);
        drawIMUPointer(gfx);
        flushDisplay(gfx->getFramebuffer());
    }
    else if (currentWatchFaceIndex == 3)
    {
        gfx->fillScreen(RGB565_BLACK);
        // test utf8 chinese text
        gfx->setUTF8Print(true);
        gfx->setTextColor(RGB565_RED);
        // u8g2_font_unifont_t_gb2312 ->16 height? 304973 bytes
        // u8g2_font_unifont_t_chinese -> 979557 bytes
        // u8g2_font_wqy12_t_gb2312b -> 12 height, 118722 bytes
        gfx->setFont(u8g2_font_wqy12_t_gb2312b);
        gfx->setCursor(80, 120);
        gfx->print("こんにちは世界!\n你好世界\n加速度计");
        gfx->setFont(FONT);
        gfx->setTextColor(RGB565_WHITE);
        if (0)
        {
            size_t lineCount = getLineCharCount(shen_nong_c_str);
            MY_LOG("lineCount: %d", lineCount);
            size_t lineStartPos = 0;
            char line_buf[256];
            for (size_t i = 0; i < lineCount; i++)
            {
                int16_t y = 8 * (i + 1);
                gfx->setCursor(100, y);
                if (y > 240)
                {
                    break;
                }
                lineStartPos = getNextLineWrapCharPos(shen_nong_c_str, lineStartPos) + 1;
                size_t nextLineWrapPos = getNextLineWrapCharPos(shen_nong_c_str, lineStartPos);
                strncpy(line_buf, shen_nong_c_str + lineStartPos, nextLineWrapPos - lineStartPos);
                line_buf[nextLineWrapPos - lineStartPos] = '\0';
                gfx->print(line_buf);
            }
        }
        gfx->setCursor(0, 0);
        gfx->print(three_body_problem);
        flushDisplay(gfx->getFramebuffer());
    }
    else
    {
        MY_LOG("Unknown watch face index: %d", currentWatchFaceIndex);
    }
}

void ThymeWatchFace::drawIMUPointer(Arduino_Canvas_6bit *gfx)
{
    // x 取负的
    int16_t x = 120 - 120 * imu_data.acc_x;
    int16_t y = 120 + 120 * imu_data.acc_y;
    // horizontal crosshair
    gfx->fillRect(x - CrosshairHalfLength, y - CrosshairHalfThickness, 2 * CrosshairHalfLength, 2 * CrosshairHalfThickness, RGB565_GREEN);
    // vertical crosshair
    gfx->fillRect(x - CrosshairHalfThickness, y - CrosshairHalfLength, 2 * CrosshairHalfThickness, 2 * CrosshairHalfLength, RGB565_GREEN);
    gfx->fillCircle(x, y, CrosshairCenterRadius, RGB565_RED);
}

void ThymeWatchFace::onUpButtonPressed()
{
    currentWatchFaceIndex++;
    // watchFaceCount
    if (currentWatchFaceIndex >= WatchFaceCount)
    {
        currentWatchFaceIndex = 0;
    }
    MY_LOG("ThymeWatchFace::onUpButtonPressed()");
}

void ThymeWatchFace::onDigitalCrownRotated(long position)
{
    lastDigitalCrownPosition = position;
}

void ThymeWatchFace::onMiddleButtonPressed()
{
    AppManager::navigateToApp<ThymeWatchMenu>();
    MY_LOG("ThymeWatchFace::onMiddleButtonPressed()");
}

void ThymeWatchFace::onDownButtonPressed()
{
    currentWatchFaceIndex--;
    if (currentWatchFaceIndex < 0)
    {
        currentWatchFaceIndex = WatchFaceCount - 1;
    }
    MY_LOG("ThymeWatchFace::onDownButtonPressed()");
}

ThymeWatchFace::~ThymeWatchFace()
{
    MY_LOG("ThymeWatchFace::~ThymeWatchFace()");
}

void ThymeWatchFace::onBackPressed()
{
    toggleBacklight();
    MY_LOG("ThymeWatchFace::onBackPressed()");
}

void ThymeWatchFace::onBackLongPressed()
{
    MY_LOG("ThymeWatchFace::onBackLongPressed()");
    enter_deep_sleep();
}

void ThymeWatchFace::renderNyanCat(Arduino_Canvas_6bit *gfx)
{
    uint16_t nyanCatFrameCount = sizeof(anim_data) / sizeof(anim_data[0]);
    MY_LOG("nyanCatFrameCount: %d", nyanCatFrameCount);
    memcpy(gfx->getFramebuffer(), anim_data[nyanCatFramePos], 240 * 240);
    nyanCatFramePos++;
    if (nyanCatFramePos >= nyanCatFrameCount)
    {
        nyanCatFramePos = 0;
    }
    // Draw hours, minutes and seconds time in string format: HH:MM:SS on the top
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
    printCenteredText(gfx, buf, RGB565_WHITE, 3, 120, 50);
    flushDisplay(gfx->getFramebuffer());
}
