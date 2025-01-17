// copied from: https://github.com/espressif/arduino-esp32/blob/master/libraries/SD_MMC/examples/SD2USBMSC/SD2USBMSC.ino
// https://github.com/espressif/arduino-esp32/blob/master/platform.txt
// build.extra_flags.esp32s3=-DARDUINO_USB_MODE={build.usb_mode} -DARDUINO_USB_CDC_ON_BOOT={build.cdc_on_boot} -DARDUINO_USB_MSC_ON_BOOT={build.msc_on_boot} -DARDUINO_USB_DFU_ON_BOOT={build.dfu_on_boot}
// menu: USB Mode: Hardware CDC and JTAG / USB-OTG (TinyUSB)

// # Official Espressif options
// menu.UploadSpeed=Upload Speed
// menu.USBMode=USB Mode
// menu.CDCOnBoot=USB CDC On Boot
// menu.MSCOnBoot=USB Firmware MSC On Boot
// menu.DFUOnBoot=USB DFU On Boot
// menu.UploadMode=Upload Mode
// menu.CPUFreq=CPU Frequency
// menu.FlashFreq=Flash Frequency
// menu.FlashMode=Flash Mode
// menu.FlashSize=Flash Size
// menu.PartitionScheme=Partition Scheme
// menu.DebugLevel=Core Debug Level
// menu.PSRAM=PSRAM
// menu.LoopCore=Arduino Runs On
// menu.EventsCore=Events Run On
// menu.MemoryType=Memory Type
// menu.EraseFlash=Erase All Flash Before Sketch Upload
// menu.JTAGAdapter=JTAG Adapter
// menu.ZigbeeMode=Zigbee Mode
// menu.PinNumbers=Pin Numbering

// esp32s3.menu.USBMode.hwcdc=Hardware CDC and JTAG
// esp32s3.menu.USBMode.hwcdc.build.usb_mode=1
// esp32s3.menu.USBMode.default=USB-OTG (TinyUSB)
// esp32s3.menu.USBMode.default.build.usb_mode=0

// esp32s3.menu.CDCOnBoot.default=Disabled
// esp32s3.menu.CDCOnBoot.default.build.cdc_on_boot=0
// esp32s3.menu.CDCOnBoot.cdc=Enabled
// esp32s3.menu.CDCOnBoot.cdc.build.cdc_on_boot=1

// esp32s3.menu.MSCOnBoot.default=Disabled
// esp32s3.menu.MSCOnBoot.default.build.msc_on_boot=0
// esp32s3.menu.MSCOnBoot.msc=Enabled (Requires USB-OTG Mode)
// esp32s3.menu.MSCOnBoot.msc.build.msc_on_boot=1

// esp32s3.menu.DFUOnBoot.default=Disabled
// esp32s3.menu.DFUOnBoot.default.build.dfu_on_boot=0
// esp32s3.menu.DFUOnBoot.dfu=Enabled (Requires USB-OTG Mode)
// esp32s3.menu.DFUOnBoot.dfu.build.dfu_on_boot=1

// esp32s3 的 SOC_USB_OTG_SUPPORTED 是 1
// 所以需要选上: USB-OTG (TinyUSB)
// 或者 PlatformIO 定义 -DARDUINO_USB_MODE=0

// #undef ARDUINO_USB_MODE
// #define ARDUINO_USB_MODE (0)
// #define SOC_USB_OTG_SUPPORTED (1)

// #if !SOC_USB_OTG_SUPPORTED || ARDUINO_USB_MODE
// #error Device does not support USB_OTG or native USB CDC/JTAG is selected
// #endif

#include <USB.h>
#include <USBMSC.h>
#include <SD_MMC.h>

#define SENSOR_POWER_PIN (9)
#define SDMMC_FREQ  (40 * 1000)

// USB Mass Storage Class (MSC) object
USBMSC msc;

int clk = 15;
int cmd = 14;
int d0 = 21;
int d1 = 18;
int d2 = 17;
int d3 = 16;
bool onebit = false; // set to false for 4-bit. 1-bit will ignore the d1-d3 pins (but d3 must be pulled high)
bool format_if_mount_failed = true;

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
  uint32_t secSize = SD_MMC.sectorSize();
  if (!secSize)
  {
    return false; // disk error
  }
  log_v("Write lba: %ld\toffset: %ld\tbufsize: %ld", lba, offset, bufsize);
  for (int x = 0; x < bufsize / secSize; x++)
  {
    uint8_t blkbuffer[secSize];
    memcpy(blkbuffer, (uint8_t *)buffer + secSize * x, secSize);
    if (!SD_MMC.writeRAW(blkbuffer, lba + x))
    {
      return false;
    }
  }
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
  uint32_t secSize = SD_MMC.sectorSize();
  if (!secSize)
  {
    return false; // disk error
  }
  log_v("Read lba: %ld\toffset: %ld\tbufsize: %ld\tsector: %lu", lba, offset, bufsize, secSize);
  for (int x = 0; x < bufsize / secSize; x++)
  {
    if (!SD_MMC.readRAW((uint8_t *)buffer + (x * secSize), lba + x))
    {
      return false; // outside of volume boundary
    }
  }
  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
  log_i("Start/Stop power: %u\tstart: %d\teject: %d", power_condition, start, load_eject);
  return true;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  if (event_base == ARDUINO_USB_EVENTS)
  {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id)
    {
    case ARDUINO_USB_STARTED_EVENT:
      Serial.println("USB PLUGGED");
      break;
    case ARDUINO_USB_STOPPED_EVENT:
      Serial.println("USB UNPLUGGED");
      break;
    case ARDUINO_USB_SUSPEND_EVENT:
      Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
      break;
    case ARDUINO_USB_RESUME_EVENT:
      Serial.println("USB RESUMED");
      break;

    default:
      break;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(0, INPUT);
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  delay(1500);
  Serial.println("setup() Starting Serial...");

  Serial.println("Mounting SDcard");
  SD_MMC.setPins(clk, cmd, d0, d1, d2, d3);
  if (!SD_MMC.begin("/sdcard", onebit, format_if_mount_failed, SDMMC_FREQ))
  {
    Serial.println("Mount Failed");
    return;
  }

  Serial.println("Initializing MSC");
  // Initialize USB metadata and callbacks for MSC (Mass Storage Class)
  msc.vendorID("ESP32");
  msc.productID("USB_MSC");
  msc.productRevision("1.0");
  msc.onRead(onRead);
  msc.onWrite(onWrite);
  msc.onStartStop(onStartStop);
  msc.mediaPresent(true);
  // eggfly added
  msc.isWritable(true); // true if writable, false if read-only

  msc.begin(SD_MMC.numSectors(), SD_MMC.sectorSize());

  Serial.println("Initializing USB");

  USB.begin();
  USB.onEvent(usbEventCallback);

  Serial.printf("Card Size: %lluMB\n", SD_MMC.totalBytes() / 1024 / 1024);
  Serial.printf("Sector: %d\tCount: %d\n", SD_MMC.sectorSize(), SD_MMC.numSectors());
}

void loop()
{
  delay(10);
  if (digitalRead(0) == LOW) {
    ESP.restart();
  }
}
