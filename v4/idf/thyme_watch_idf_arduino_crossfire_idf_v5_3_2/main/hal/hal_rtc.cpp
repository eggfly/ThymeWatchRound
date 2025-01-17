
#include <RTC_RX8025T.h>
#include <RTClib.h>

// tmElements_t tm;
const char *dayOfWeekName[] =
    {
        "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

const char *monthName[] =
    {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

void printDateTime(tmElements_t tm)
{
  char buf[256];
  snprintf(buf, sizeof(buf), "Date: %d %s %d %s Time: %d:%d:%d",
           tmYearToCalendar(tm.Year), monthName[tm.Month], tm.Day, dayOfWeekName[tm.Wday - 1],
           tm.Hour, tm.Minute, tm.Second);
  ESP_LOGI("HAL_RTC", "%s", buf);

  Serial.print("Date: ");
  // Time library macro for convenient Year conversion to calendar format
  Serial.print(tmYearToCalendar(tm.Year));
  Serial.print(" ");
  Serial.print(monthName[tm.Month]);
  Serial.print(" ");
  if (tm.Day < 10)
    Serial.print("0");
  Serial.print(tm.Day);
  Serial.print(" ");
  // tm.Wday stores values from 1 to 7, so we have to subtract 1 to correctly read the names from the array
  Serial.print(dayOfWeekName[tm.Wday - 1]);

  Serial.print("     Time: ");
  if (tm.Hour < 10)
    Serial.print("0");
  Serial.print(tm.Hour);
  Serial.print(":");
  if (tm.Minute < 10)
    Serial.print("0");
  Serial.print(tm.Minute);
  Serial.print(":");
  if (tm.Second < 10)
    Serial.print("0");
  Serial.println(tm.Second);
}

tmElements_t readRTC()
{
  tmElements_t tm;
  RTC_RX8025T.read(tm);
  // printDateTime(tm);
  return tm;
}

bool forceAdjust = false;

void initRTC()
{
  // RX8025T initialization
  RTC_RX8025T.init();
  tmElements_t tm = readRTC();
  // time_t rtcTime = makeTime(tm);
  Serial.printf("RTC Year: %d\n", tm.Year);

  DateTime compileDateTime(F(__DATE__), F(__TIME__));
  // setTime(10, 23, 30, 25, 10, 22);
  auto ts = compileDateTime.unixtime();
  // 默认是 2000 年, 默认 Year 是 30
  if (tm.Year <= 30 || forceAdjust)
  {
    // NEED adjust time
    // The Time library is a software RTC_RX8025T.
    //"system time" it is referring to the current time according to the software RTC_RX8025T.
    // Set the system time to 10h 23m 30s on 25 Oct 2022
    // 这一步不会写到 I2C
    setTime(ts);
    // Set the RTC from the system time
    RTC_RX8025T.set(now());
    Serial.printf("RTC_RX8025T set to %s %s, ts=%ld\n", __DATE__, __TIME__, ts);
  }
  else
  {
    Serial.println("RTC_RX8025T No need to adjust time.");
  }
}
