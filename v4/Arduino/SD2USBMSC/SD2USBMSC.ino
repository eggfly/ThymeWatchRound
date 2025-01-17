#if !SOC_USB_OTG_SUPPORTED || ARDUINO_USB_MODE
#error Device does not support USB_OTG or native USB CDC/JTAG is selected
#endif

// NEED: USB CDC On Boot = "Enabled"
#include <USB.h>
#include <USBMSC.h>
#include <SD_MMC.h>

#define SENSOR_POWER_PIN (9)

// USB Mass Storage Class (MSC) object
USBMSC msc;

int clk = 15;
int cmd = 14;
int d0 = 21;
int d1 = 18;
int d2 = 17;
int d3 = 16;
bool onebit = false;  // set to false for 4-bit. 1-bit will ignore the d1-d3 pins (but d3 must be pulled high)
bool format_if_mount_failed = true;

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
  uint32_t secSize = SD_MMC.sectorSize();
  if (!secSize) {
    return false;  // disk error
  }
  log_v("Write lba: %ld\toffset: %ld\tbufsize: %ld", lba, offset, bufsize);
  for (int x = 0; x < bufsize / secSize; x++) {
    uint8_t blkbuffer[secSize];
    memcpy(blkbuffer, (uint8_t *)buffer + secSize * x, secSize);
    if (!SD_MMC.writeRAW(blkbuffer, lba + x)) {
      return false;
    }
  }
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
  uint32_t secSize = SD_MMC.sectorSize();
  if (!secSize) {
    return false;  // disk error
  }
  log_v("Read lba: %ld\toffset: %ld\tbufsize: %ld\tsector: %lu", lba, offset, bufsize, secSize);
  for (int x = 0; x < bufsize / secSize; x++) {
    if (!SD_MMC.readRAW((uint8_t *)buffer + (x * secSize), lba + x)) {
      return false;  // outside of volume boundary
    }
  }
  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
  log_i("Start/Stop power: %u\tstart: %d\teject: %d", power_condition, start, load_eject);
  return true;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id) {
      case ARDUINO_USB_STARTED_EVENT: Serial.println("USB PLUGGED"); break;
      case ARDUINO_USB_STOPPED_EVENT: Serial.println("USB UNPLUGGED"); break;
      case ARDUINO_USB_SUSPEND_EVENT: Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en); break;
      case ARDUINO_USB_RESUME_EVENT:  Serial.println("USB RESUMED"); break;

      default: break;
    }
  }
}

void setup() {
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  Serial.begin(115200);
  Serial.println("Starting Serial");

  Serial.println("Mounting SDcard");
  SD_MMC.setPins(clk, cmd, d0, d1, d2, d3);
  if (!SD_MMC.begin("/sdcard", onebit, format_if_mount_failed)) {
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
  msc.isWritable(true);  // true if writable, false if read-only

  msc.begin(SD_MMC.numSectors(), SD_MMC.sectorSize());

  Serial.println("Initializing USB");

  USB.begin();
  USB.onEvent(usbEventCallback);

  Serial.printf("Card Size: %lluMB\n", SD_MMC.totalBytes() / 1024 / 1024);
  Serial.printf("Sector: %d\tCount: %d\n", SD_MMC.sectorSize(), SD_MMC.numSectors());
}

void loop() {
  delay(-1);
}
