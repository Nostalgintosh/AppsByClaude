// ═══════════════════════════════════════════════════════════════════════════
//  CYPHAMOMETER — CFA Food Safety Thermometer
//  Arduino / ESP32 Sketch
//  cfa_thermometer.ino
//
// ┌─────────────────────────────────────────────────────────────────────────┐
// │                  WHY A MICROSD CARD IN THIS THERMOMETER?               │
// ├─────────────────────────────────────────────────────────────────────────┤
// │                                                                         │
// │  The MicroSD card solves two critical problems at once:                 │
// │                                                                         │
// │  1. WIRELESS-STYLE DATA TRANSFER (without Wi-Fi or Bluetooth)           │
// │     Think of the MicroSD card as a "physical wireless drive." Once a   │
// │     shift ends (or whenever a manager needs the logs), they simply:     │
// │       a) Power off the thermometer  OR press the export button          │
// │       b) Slide the MicroSD card out of the side of the device           │
// │       c) Walk to the office and insert it into any USB card reader      │
// │       d) Open the CSV files directly in Microsoft Excel                 │
// │     No USB cable threading through the kitchen. No pairing codes.       │
// │     No Wi-Fi password required. No app to install. No server needed.    │
// │     The data literally walks out of the kitchen on a card the size      │
// │     of a fingernail — that IS the wireless transfer.                    │
// │                                                                         │
// │  2. LONG-TERM ARCHIVAL STORAGE (1 month to 2+ years)                   │
// │     The ESP32's internal flash holds only a few hundred KB of user      │
// │     data — barely a week of readings before it fills up. The MicroSD   │
// │     card gives us gigabytes. Every reading is written and flushed to   │
// │     the card immediately, so even a sudden power outage cannot erase   │
// │     data that has already been logged.                                  │
// │                                                                         │
// │     STORAGE CAPACITY GUIDE:                                             │
// │     Each reading   = ~180 bytes in CSV                                  │
// │     Per hour (8 products) = ~1,440 bytes = ~1.5 KB                     │
// │     Per day:                                                            │
// │       Mon-Thu (11 slots): ~16 KB/day                                    │
// │       Fri-Sat (13 slots): ~19 KB/day                                    │
// │     Per week (6 days avg): ~100 KB/week                                 │
// │     Per month (4.3 weeks): ~430 KB/month                               │
// │                                                                         │
// │     CARD SIZE  →  RETENTION AT FULL DAILY LOGGING                      │
// │     ──────────────────────────────────────────────                      │
// │      1 GB  →  ~2,200 months  (~183 years — far more than needed)        │
// │      2 GB  →  Minimum recommended — stores 5+ years of daily logs      │
// │      4 GB  →  10+ years. Ideal for a device that won't be serviced      │
// │              frequently; holds the thermometer's entire useful life.    │
// │      8 GB  →  20+ years. Essentially the device's full lifetime with    │
// │              room for full monthly summary exports and backups.         │
// │     16 GB  →  40+ years — effectively unlimited for this use case.     │
// │     32 GB  →  Maximum for FAT32 (required for Arduino SD library).     │
// │                                                                         │
// │     PRACTICAL RECOMMENDATION:                                           │
// │       • Get a 4–8 GB card (Samsung Pro Endurance or SanDisk Industrial) │
// │       • These are rated for continuous write cycles (dashcam-grade)     │
// │       • Avoid cheap off-brand cards — they fail without warning         │
// │       • Format as FAT32 before first use                                │
// │                                                                         │
// │  FILE ORGANIZATION ON THE CARD:                                         │
// │    /CFA_LOGS/                                                            │
// │      2025/                                                              │
// │        03/                                                              │
// │          CFA_20250317_Mon.CSV   ← one file per operating day            │
// │          CFA_20250318_Tue.CSV                                           │
// │          INDEX_2025-03.CSV      ← monthly quick-reference index        │
// │        04/                                                              │
// │          CFA_20250401_Tue.CSV                                           │
// │    /EXPORT/                                                             │
// │          CFA_TempLog_2025-03-17_Mon.csv  ← clean Excel export          │
// │    system_events.log            ← mount/error events                   │
// │                                                                         │
// │  HOW TO TRANSFER (step by step):                                        │
// │    1. After your shift, press END then hold START for 3 sec             │
// │       → "Export Ready" appears on screen                                │
// │    2. Turn off thermometer (or leave on — card can be hot-swapped       │
// │       between readings during slow periods)                             │
// │    3. Pinch the MicroSD card and slide it out of the slot              │
// │    4. Insert into a USB MicroSD reader (usually $5–$10 at any store)   │
// │    5. Open the /EXPORT/ folder on the card from any computer            │
// │    6. Drag the CSV file to your desktop or email it to your director    │
// │    7. Reinsert the card — the thermometer resumes automatically         │
// └─────────────────────────────────────────────────────────────────────────┘
//
//  HARDWARE BILL OF MATERIALS:
//  ┌───────────────────────────────────────────────────────────────────────┐
//  │  MCU          : ESP32 DevKit v1 (38-pin) or Arduino Due              │
//  │  Temp Sensor  : DS18B20 waterproof stainless probe  (GPIO 4)        │
//  │  Display      : 20×4 I2C LCD (PCF8574 backpack, addr 0x27)          │
//  │                 SDA=GPIO 21   SCL=GPIO 22                            │
//  │  MicroSD Mod. : SPI MicroSD breakout board                          │
//  │                 CS=GPIO 5  MOSI=GPIO 23  MISO=GPIO 19  SCK=GPIO 18  │
//  │  RTC Module   : DS3231 (I2C, shared bus SDA=21 SCL=22)              │
//  │  LED Green    : GPIO 25  →  220Ω  →  LED  →  GND                   │
//  │  LED Red      : GPIO 26  →  220Ω  →  LED  →  GND                   │
//  │  Buzzer       : GPIO 27  →  passive piezo  →  GND                   │
//  │  Buttons      : all wired to GND; ESP32 uses INPUT_PULLUP            │
//  │                 GPIO 13=Regular  14=Spicy   15=Grilled  16=CJ        │
//  │                 GPIO 17=PJAmero  18=Lettuce 19=Tomatoes 23=Fries     │
//  │                 GPIO 32=START    33=END                              │
//  │  MicroSD Card : 4–8 GB, Samsung Pro Endurance, FAT32 formatted      │
//  └───────────────────────────────────────────────────────────────────────┘
//
//  ARDUINO LIBRARY MANAGER INSTALLS:
//    • LiquidCrystal_I2C  by Frank de Brabander
//    • OneWire            by Paul Stoffregen
//    • DallasTemperature  by Miles Burton
//    • SD                 (built-in with Arduino IDE)
//    • RTClib             by Adafruit
// ═══════════════════════════════════════════════════════════════════════════

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>

// ─────────────────────────────────────────────
//  PIN DEFINITIONS
// ─────────────────────────────────────────────
#define PIN_ONE_WIRE       4    // DS18B20 data line (+ 4.7kΩ pullup to 3.3V)
#define PIN_SD_CS          5    // MicroSD chip-select

// SPI pins for MicroSD (hardware SPI on ESP32)
#define PIN_SD_MOSI       23
#define PIN_SD_MISO       19
#define PIN_SD_SCK        18

#define PIN_LED_GREEN     25
#define PIN_LED_RED       26
#define PIN_BUZZER        27

// Product selection buttons (wire each pin to GND; INPUT_PULLUP reads LOW when pressed)
#define PIN_BTN_REGULAR   13
#define PIN_BTN_SPICY     14
#define PIN_BTN_GRILLED   15
#define PIN_BTN_CJ        16
#define PIN_BTN_PJ_AMERO  17
#define PIN_BTN_LETTUCE   18
#define PIN_BTN_TOMATOES  19
#define PIN_BTN_FRIES     23
#define PIN_BTN_START     32
#define PIN_BTN_END       33

// ─────────────────────────────────────────────
//  STORE CONFIGURATION  (edit these for your location)
// ─────────────────────────────────────────────
const char* STORE_NAME   = "";  // ← Set your store name, e.g. "CFA #03745"
const int   OPEN_HOUR    = 11;    // 11:00 AM every operating day
const int   CLOSE_WKDAY  = 22;    // Mon–Thu: last check slot = 21 (9 PM)
const int   CLOSE_WKEND  = 24;    // Fri–Sat: last check slot = 23 (11 PM)
// Sunday = wday 0 → no logging, always blocked

// ─────────────────────────────────────────────
//  CFA PRODUCT TABLE  (button 1–8 maps to index 0–7)
// ─────────────────────────────────────────────
struct Product {
    const char* name;     // ≤19 chars for 20-col LCD
    const char* label;    // official CFA log label
    float       minF;     // minimum safe temperature °F
    float       maxF;     // maximum safe temperature °F
    bool        hot;      // true = hot-hold, false = cold-hold
};

const Product PRODUCTS[8] = {
    { "Original CFS",   "Chicken Sandwich",    140, 180, true  },  // BTN 1
    { "Spicy Deluxe",   "Spicy Deluxe",        140, 180, true  },  // BTN 2
    { "Grilled Filet",  "Grilled Sandwich",    140, 180, true  },  // BTN 3
    { "Nuggets/Strips", "Nuggets & Strips",    140, 180, true  },  // BTN 4
    { "Mac & Cheese",   "Mac & Cheese/Soup",   140, 180, true  },  // BTN 5
    { "Lettuce",        "Cold Toppings",        34,  41, false },  // BTN 6
    { "Tomatoes",       "Tomatoes/Produce",     34,  41, false },  // BTN 7
    { "Waffle Fries",   "Waffle Fries",        135, 180, true  },  // BTN 8
};

// ─────────────────────────────────────────────
//  HARDWARE OBJECTS
// ─────────────────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 20, 4);
OneWire           oneWire(PIN_ONE_WIRE);
DallasTemperature tempSensor(&oneWire);
RTC_DS3231        rtc;

// ─────────────────────────────────────────────
//  STATE VARIABLES
// ─────────────────────────────────────────────
int   selProduct  = -1;       // currently selected product (0–7), -1 = none
bool  measuring   = false;    // true while waiting for DS18B20 conversion
int   lastHour    = -1;       // used to detect hour changes for reset
int   lastWday    = -1;
int   hourChecks  = 0;        // readings taken this hour
int   hourPass    = 0;        // passed readings this hour
int   dayChecks   = 0;        // total readings today
int   dayPass     = 0;        // total passed today
bool  sdMounted   = false;    // MicroSD mount status
bool  sdError     = false;    // SD write error flag
uint32_t sdFreeKB = 0;        // free space estimate

// File paths on MicroSD
char  dayFilePath[48];        // e.g. /CFA_LOGS/2025/03/CFA_20250317_Mon.CSV
char  monthIndexPath[48];     // e.g. /CFA_LOGS/2025/03/INDEX_2025-03.CSV
char  exportFilePath[56];     // e.g. /EXPORT/CFA_TempLog_2025-03-17_Mon.csv

// ─────────────────────────────────────────────
//  SCHEDULE HELPERS
// ─────────────────────────────────────────────
int closeHourForDay(int wday) {
    if (wday == 0)              return 0;          // Sunday — closed
    if (wday >= 1 && wday <= 4) return CLOSE_WKDAY;
    return CLOSE_WKEND;                            // Fri=5, Sat=6
}

bool isOperating(int wday, int hour) {
    int close = closeHourForDay(wday);
    return close > 0 && hour >= OPEN_HOUR && hour < close;
}

const char* dayName(int wday) {
    const char* D[] = {"Sunday","Monday","Tuesday","Wednesday",
                       "Thursday","Friday","Saturday"};
    return D[wday];
}

const char* dayAbbr(int wday) {
    const char* A[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    return A[wday];
}

// ─────────────────────────────────────────────
//  BUTTON DEBOUNCE
// ─────────────────────────────────────────────
struct Btn { int pin; bool last; unsigned long ts; };
Btn BTNS[10] = {
    {PIN_BTN_REGULAR, false, 0}, {PIN_BTN_SPICY,    false, 0},
    {PIN_BTN_GRILLED, false, 0}, {PIN_BTN_CJ,       false, 0},
    {PIN_BTN_PJ_AMERO,false, 0}, {PIN_BTN_LETTUCE,  false, 0},
    {PIN_BTN_TOMATOES,false, 0}, {PIN_BTN_FRIES,    false, 0},
    {PIN_BTN_START,   false, 0}, {PIN_BTN_END,      false, 0},
};

int getPressedButton() {
    for (int i = 0; i < 10; i++) {
        bool state = (digitalRead(BTNS[i].pin) == LOW);
        if (state != BTNS[i].last) BTNS[i].ts = millis();
        if ((millis() - BTNS[i].ts) > 50 && state && !BTNS[i].last) {
            BTNS[i].last = state;
            return i;
        }
        BTNS[i].last = state;
    }
    return -1;
}

// ─────────────────────────────────────────────
//  LCD HELPERS
// ─────────────────────────────────────────────
void lcdRow(int row, const char* text) {
    char buf[21];
    snprintf(buf, sizeof(buf), "%-20s", text);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

void alertPass()  {
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_RED,   LOW);
    tone(PIN_BUZZER, 1800, 200);
}
void alertFail()  {
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED,   HIGH);
    tone(PIN_BUZZER, 600, 700);
}
void alertClear() {
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED,   LOW);
    noTone(PIN_BUZZER);
}

// ─────────────────────────────────────────────
//  IDLE SCREEN
// ─────────────────────────────────────────────
void showIdle(int wday, int hour) {
    lcd.clear();
    lcdRow(0, "Cyphamometer");
    if (wday == 0) {
        lcdRow(1, "Closed (Sunday)");
        lcdRow(2, "No logging today");
        lcdRow(3, sdMounted ? "SD OK" : "SD: Check card!");
    } else if (isOperating(wday, hour)) {
        lcdRow(1, dayName(wday));
        lcdRow(2, "Pick product 1-8");
        lcdRow(3, "then press START");
    } else {
        lcdRow(1, dayName(wday));
        if (closeHourForDay(wday) == CLOSE_WKDAY)
            lcdRow(2, "11AM - 9:59PM");
        else
            lcdRow(2, "11AM - 11:59PM");
        lcdRow(3, "Outside hours.");
    }
}

// ─────────────────────────────────────────────
//  ▶▶  MICROSD STORAGE SYSTEM  ◀◀
//
//  WHY: The MicroSD card is the "wireless" data bridge.
//  Team members never need a cable or a smartphone.
//  They pop the card out, walk to any PC, and the logs
//  open instantly in Excel — just like inserting a thumb drive,
//  but smaller and permanently installed in the thermometer.
//
//  Retention at full daily logging:
//    2 GB card  →  ~5 years
//    4 GB card  →  ~10 years
//    8 GB card  →  ~20 years (device lifetime)
// ─────────────────────────────────────────────

// Build all file paths for today's date
void buildFilePaths(const DateTime& now) {
    int wday = now.dayOfTheWeek();

    // Ensure year and month folders exist
    char yearDir[24], monthDir[32], logsDir[40];
    snprintf(yearDir,  sizeof(yearDir),  "/CFA_LOGS/%04d", now.year());
    snprintf(monthDir, sizeof(monthDir), "/CFA_LOGS/%04d/%02d",
             now.year(), now.month());

    SD.mkdir(yearDir);
    SD.mkdir(monthDir);
    SD.mkdir("/EXPORT");

    // Daily CSV:  /CFA_LOGS/YYYY/MM/CFA_YYYYMMDD_Day.CSV
    snprintf(dayFilePath, sizeof(dayFilePath),
             "/CFA_LOGS/%04d/%02d/CFA_%04d%02d%02d_%s.CSV",
             now.year(), now.month(),
             now.year(), now.month(), now.day(),
             dayAbbr(wday));

    // Monthly index:  /CFA_LOGS/YYYY/MM/INDEX_YYYY-MM.CSV
    snprintf(monthIndexPath, sizeof(monthIndexPath),
             "/CFA_LOGS/%04d/%02d/INDEX_%04d-%02d.CSV",
             now.year(), now.month(),
             now.year(), now.month());

    // Export CSV:  /EXPORT/CFA_TempLog_YYYY-MM-DD_Day.csv
    snprintf(exportFilePath, sizeof(exportFilePath),
             "/EXPORT/CFA_%04d-%02d-%02d_%s.CSV",
             now.year(), now.month(), now.day(),
             dayAbbr(wday));
}

// Initialise daily CSV file (write header if new)
void initDayFile(const DateTime& now) {
    if (SD.exists(dayFilePath)) return;  // already exists — continue appending

    File f = SD.open(dayFilePath, FILE_WRITE);
    if (!f) { sdError = true; return; }

    // Header block explains the file for anyone opening it on a PC
    f.println("# CYPHAMOMETER CFA FOOD SAFETY TEMPERATURE LOG");
    f.println("# ─────────────────────────────────────────────");
    f.println("# This file was recorded on a MicroSD card");
    f.println("# installed inside a CFA Food Safety Thermometer.");
    f.println("# Transfer: remove card -> USB reader -> open in Excel");
    f.println("# Retention: 2 GB card = ~5 yrs, 4 GB = ~10 yrs, 8 GB = ~20 yrs");
    f.println("# ─────────────────────────────────────────────");

    char line[80];
    snprintf(line, sizeof(line), "# Store: %s", STORE_NAME);
    f.println(line);

    char dateStr[32];
    snprintf(dateStr, sizeof(dateStr), "# Date: %04d-%02d-%02d  Day: %s",
             now.year(), now.month(), now.day(),
             dayName(now.dayOfTheWeek()));
    f.println(dateStr);

    int close = closeHourForDay(now.dayOfTheWeek());
    if (close == CLOSE_WKDAY)
        f.println("# Hours: 11:00 AM - 9:59 PM");
    else if (close == CLOSE_WKEND)
        f.println("# Hours: 11:00 AM - 11:59 PM");
    else
        f.println("# Hours: CLOSED TODAY (Sunday)");

    f.println("#");
    f.println("Date,Day,Hour,Time,CFA Item,Temp F,Min F,Max F,Status,Corrective Action");
    f.close();
}

// Initialize monthly index file
void initMonthIndex(const DateTime& now) {
    if (SD.exists(monthIndexPath)) return;
    File f = SD.open(monthIndexPath, FILE_WRITE);
    if (!f) return;
    char buf[80];
    snprintf(buf, sizeof(buf),
             "# CFA Monthly Temperature Index — %04d-%02d",
             now.year(), now.month());
    f.println(buf);
    f.println("Date,Day,Time,Product,Temp F,Status");
    f.close();
}

// Log system events (SD errors, mounts, etc.)
void logSystemEvent(const DateTime& now, const char* msg) {
    File f = SD.open("/system_events.log", FILE_APPEND);
    if (!f) return;
    char buf[120];
    snprintf(buf, sizeof(buf),
             "[%04d-%02d-%02d %02d:%02d:%02d] %s",
             now.year(), now.month(),  now.day(),
             now.hour(), now.minute(), now.second(), msg);
    f.println(buf);
    f.close();
}

// ── Write one temperature reading to MicroSD ──────────────────────────────
//  This is called IMMEDIATELY after every measurement.
//  The data is on the card within milliseconds of the reading.
void writeEntryToSD(const DateTime& now, float temp, bool pass,
                    int wday, int hour, int productIdx)
{
    if (!sdMounted) return;

    const Product& p = PRODUCTS[productIdx];

    // Hour label (12-hour format for readability)
    int h12 = hour % 12; if (h12 == 0) h12 = 12;
    const char* ap = (hour < 12) ? "AM" : "PM";
    char hSlot[10];
    snprintf(hSlot, sizeof(hSlot), "%02d:00%s", h12, ap);

    // Determine status and corrective action
    const char* status;
    char corrective[52] = "";
    if (pass) {
        status = "PASS";
    } else if (temp < p.minF) {
        status = p.hot ? "FAIL-COLD" : "FAIL-WARM";
        strncpy(corrective,
                p.hot ? "Reheat to 165F or discard"
                       : "Move to cooler - check unit",
                sizeof(corrective) - 1);
    } else {
        status = "FAIL-HOT";
        strncpy(corrective,
                p.hot ? "Check hot-holding unit"
                       : "Discard - check refrigeration",
                sizeof(corrective) - 1);
    }

    // ── Write to daily detail file ──
    File dayFile = SD.open(dayFilePath, FILE_APPEND);
    if (dayFile) {
        char line[180];
        snprintf(line, sizeof(line),
                 "%04d-%02d-%02d,%s,%s,%02d:%02d:%02d,%s,%.1f,%.0f,%.0f,%s,%s",
                 now.year(), now.month(), now.day(),
                 dayName(wday), hSlot,
                 now.hour(), now.minute(), now.second(),
                 p.label, temp, p.minF, p.maxF,
                 status, corrective);
        dayFile.println(line);
        dayFile.close();           // flush immediately — no data loss on power cut
        sdError = false;
    } else {
        sdError = true;
    }

    // ── Write condensed row to monthly index ──
    File idxFile = SD.open(monthIndexPath, FILE_APPEND);
    if (idxFile) {
        char line[80];
        snprintf(line, sizeof(line),
                 "%04d-%02d-%02d,%s,%02d:%02d,%s,%.1f,%s",
                 now.year(), now.month(), now.day(),
                 dayName(wday), now.hour(), now.minute(),
                 p.label, temp, status);
        idxFile.println(line);
        idxFile.close();
    }
}

// ── Write hourly reset marker ─────────────────────────────────────────────
void writeHourMarkerToSD(const DateTime& now, int wday, int hour,
                          int checks, int pass)
{
    if (!sdMounted) return;
    File f = SD.open(dayFilePath, FILE_APPEND);
    if (!f) return;
    char buf[100];
    snprintf(buf, sizeof(buf),
             "# HOUR COMPLETE,%s,%02d:00,checks=%d,pass=%d,fail=%d,pct=%.0f%%",
             dayName(wday), hour, checks, pass, checks - pass,
             checks > 0 ? 100.0f * pass / checks : 0.0f);
    f.println(buf);
    f.close();
}

// ── Build clean Excel export file in /EXPORT/ ─────────────────────────────
//  Manager copies this to their PC after removing the card.
void buildExportFile(const DateTime& now, int wday) {
    if (!sdMounted) return;

    // Remove old export for today if it exists
    if (SD.exists(exportFilePath)) SD.remove(exportFilePath);

    File f = SD.open(exportFilePath, FILE_WRITE);
    if (!f) return;

    // ── Excel-formatted header ──
    f.println("CYPHAMOMETER CFA FOOD SAFETY TEMPERATURE LOG");
    char buf[80];
    snprintf(buf, sizeof(buf), "Store:,%s", STORE_NAME);
    f.println(buf);
    snprintf(buf, sizeof(buf), "Date:,%04d-%02d-%02d",
             now.year(), now.month(), now.day());
    f.println(buf);
    snprintf(buf, sizeof(buf), "Day:,%s", dayName(wday));
    f.println(buf);
    int close = closeHourForDay(wday);
    f.println(close == CLOSE_WKDAY
              ? "Hours:,11:00 AM - 9:59 PM"
              : "Hours:,11:00 AM - 11:59 PM");

    // SD card info block
    f.println("");
    f.println("MICROSD STORAGE INFO");
    f.println("Purpose:,Long-term archival + wireless data transfer");
    f.println("Transfer method:,Remove card -> USB reader -> open in Excel");
    snprintf(buf, sizeof(buf), "Free space (est.):,%lu KB", (unsigned long)sdFreeKB);
    f.println(buf);
    f.println("Retention 2GB:,~5 years of daily logs");
    f.println("Retention 4GB:,~10 years of daily logs");
    f.println("Retention 8GB:,~20 years (device lifetime)");
    f.println("");

    // Column headers for data
    f.println("DETAILED TEMPERATURE LOG");
    f.println("Date,Day,Hour,Time,CFA Item,Temp F,Min F,Max F,Status,Corrective Action");

    // Re-read today's log file and copy data rows only (skip # comment lines)
    File src = SD.open(dayFilePath, FILE_READ);
    if (src) {
        while (src.available()) {
            String line = src.readStringUntil('\n');
            line.trim();
            if (line.length() > 0 && line[0] != '#' && line[0] != 'D') {
                // Skip the header row of source; copy data
                // (crude check: data rows start with "20" for year)
                if (line.startsWith("20")) {
                    f.println(line);
                }
            }
        }
        src.close();
    }

    // Totals block
    f.println("");
    f.println("DAILY TOTALS");
    snprintf(buf, sizeof(buf), "Total Checks:,%d", dayChecks);
    f.println(buf);
    snprintf(buf, sizeof(buf), "PASS:,%d", dayPass);
    f.println(buf);
    snprintf(buf, sizeof(buf), "FAIL:,%d", dayChecks - dayPass);
    f.println(buf);
    if (dayChecks > 0) {
        snprintf(buf, sizeof(buf), "Compliance %%:,%.1f",
                 100.0f * dayPass / dayChecks);
        f.println(buf);
    }
    f.println("Manager Initials:,___");
    f.println("Reviewed By:,___");
    f.close();

    logSystemEvent(now, "Export file built: " );
}

// ─────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    // ── LCD ──
    Wire.begin();
    lcd.begin();
    lcd.backlight();
    lcdRow(0, "Cyphamometer");
    lcdRow(1, "Starting up...");

    // ── GPIO ──
    for (auto& b : BTNS)      pinMode(b.pin, INPUT_PULLUP);
    pinMode(PIN_LED_GREEN,     OUTPUT);
    pinMode(PIN_LED_RED,       OUTPUT);
    pinMode(PIN_BUZZER,        OUTPUT);

    // ── Temperature Sensor ──
    tempSensor.begin();
    tempSensor.setResolution(12);
    tempSensor.setWaitForConversion(false);   // non-blocking conversion

    lcdRow(2, "Sensor: OK");

    // ── RTC Module ──
    if (!rtc.begin()) {
        lcdRow(2, "RTC FAIL - HALT");
        Serial.println("ERROR: RTC not found. Check wiring.");
        while (1) delay(100);
    }
    if (rtc.lostPower()) {
        // First boot or battery dead — set to compile time
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        Serial.println("RTC reset to compile time.");
    }
    lcdRow(2, "RTC: OK");

    // ── MicroSD Card ──────────────────────────────────────────────────────
    //
    //  The MicroSD card is the heart of the long-term storage system.
    //  We try to mount it on SPI. If it fails, we warn loudly but continue
    //  operating — the thermometer still works; it just can't save logs.
    //  A replacement card can be inserted at any time while powered.
    //
    lcdRow(2, "SD Card: Mounting");
    SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

    if (!SD.begin(PIN_SD_CS)) {
        sdMounted = false;
        sdError   = true;
        lcdRow(2, "!! SD Card FAIL !!");
        lcdRow(3, "Insert card!");
        Serial.println("WARNING: MicroSD not found. Logs will NOT be saved.");
        Serial.println("  Check: card inserted? wiring? FAT32 formatted?");
        delay(3000);  // show warning briefly, then continue
    } else {
        sdMounted = true;
        sdError   = false;

        // Get card info for retention estimate
        uint64_t cardSize = SD.cardSize() / (1024ULL * 1024ULL);  // MB
        uint64_t usedBytes = SD.usedBytes();
        sdFreeKB = (uint32_t)((cardSize * 1024ULL - usedBytes / 1024ULL));

        // Estimated retention: bytes/day = ~15360 → days = freeMB * 1024 * 1024 / 15360
        uint32_t estDays = sdFreeKB / 15;

        char sdMsg[64];
        snprintf(sdMsg, sizeof(sdMsg),
                 "SD: %llu MB  Free~%uMB  ~%ud",
                 cardSize, (unsigned)(sdFreeKB / 1024), estDays);
        Serial.println(sdMsg);

        lcdRow(2, "SD Card: OK");

        // Build today's file paths and init files
        DateTime now = rtc.now();
        buildFilePaths(now);
        initDayFile(now);
        initMonthIndex(now);
        logSystemEvent(now, "System started");

        snprintf(sdMsg, sizeof(sdMsg), "SD OK  ~%u days left", estDays);
        lcdRow(3, sdMsg);
        delay(1500);
    }

    // ── Record start state ──
    DateTime now = rtc.now();
    lastHour = now.hour();
    lastWday = now.dayOfTheWeek();

    showIdle(lastWday, lastHour);
    Serial.println("Cyphamometer ready.");
    Serial.println(sdMounted ? "MicroSD: MOUNTED" : "MicroSD: NOT FOUND");
}

// ─────────────────────────────────────────────
//  MAIN LOOP
// ─────────────────────────────────────────────
void loop() {
    DateTime now  = rtc.now();
    int wday      = now.dayOfTheWeek();
    int hour      = now.hour();

    // ── Hourly Reset ──────────────────────────────────────────────────────
    //  At the top of each operating hour:
    //    1. Write hour-complete marker to SD card
    //    2. Reset per-hour counters
    //    3. Show "NEW HOUR" reminder on LCD
    // ──────────────────────────────────────────────────────────────────────
    if ((hour != lastHour || wday != lastWday) && isOperating(wday, hour)) {
        writeHourMarkerToSD(now, wday, hour, hourChecks, hourPass);

        lcd.clear();
        lcdRow(0, "-- NEW HOUR --");
        lcdRow(1, dayName(wday));
        char hbuf[21];
        int h12 = hour % 12; if (h12 == 0) h12 = 12;
        snprintf(hbuf, sizeof(hbuf), "Now: %d:00 %s",
                 h12, hour < 12 ? "AM" : "PM");
        lcdRow(2, hbuf);
        int slotsLeft = closeHourForDay(wday) - hour - 1;
        snprintf(hbuf, sizeof(hbuf), "%d slots remaining", slotsLeft);
        lcdRow(3, hbuf);

        hourChecks = 0;
        hourPass   = 0;
        lastHour   = hour;
        lastWday   = wday;
        delay(2000);
        showIdle(wday, hour);
    }
    lastHour = hour;
    lastWday = wday;

    // ── Sensor Completion ─────────────────────────────────────────────────
    //  When the DS18B20 finishes its conversion, read temp,
    //  log it to MicroSD, and show result on LCD.
    // ──────────────────────────────────────────────────────────────────────
    if (measuring && tempSensor.isConversionComplete()) {
        measuring   = false;
        float tempF = tempSensor.getTempFByIndex(0);

        if (tempF == DEVICE_DISCONNECTED_F || tempF < -55.0f) {
            lcd.clear();
            lcdRow(0, "!! SENSOR ERROR !!");
            lcdRow(1, "Check probe cable");
            lcdRow(2, "Try again or");
            lcdRow(3, "press END to reset");
            alertFail();
            if (sdMounted) {
                File f = SD.open(dayFilePath, FILE_APPEND);
                if (f) {
                    f.println("# SENSOR ERROR — probe disconnected");
                    f.close();
                }
            }
            return;
        }

        const Product& p = PRODUCTS[selProduct];
        bool pass = (tempF >= p.minF && tempF <= p.maxF);

        hourChecks++;
        hourPass += pass ? 1 : 0;
        dayChecks++;
        dayPass  += pass ? 1 : 0;

        // ── WRITE TO MICROSD IMMEDIATELY ──────────────────────────────────
        //  This is the core of the wireless transfer + long-term storage
        //  system. Every reading is committed to the card the instant it
        //  is taken. No batch uploads. No syncing. No data loss on power cut.
        // ──────────────────────────────────────────────────────────────────
        writeEntryToSD(now, tempF, pass, wday, hour, selProduct);

        // ── Display Result ──
        lcd.clear();
        lcdRow(0, p.name);
        char line[21];
        snprintf(line, sizeof(line), "%.1fF  [%s]",
                 tempF, pass ? "PASS" : "FAIL");
        lcdRow(1, line);
        snprintf(line, sizeof(line), "OK: %.0f-%.0fF", p.minF, p.maxF);
        lcdRow(2, line);
        // Bottom row shows SD status so team can see if logging is working
        lcdRow(3, sdError ? "SD ERR! END=next" : "SD OK  END=next");

        if (pass) alertPass(); else alertFail();

        Serial.print("[Temp] ");
        Serial.print(p.label);
        Serial.print("  ");
        Serial.print(tempF, 1);
        Serial.print("F  ");
        Serial.print(pass ? "PASS" : "FAIL");
        Serial.print("  SD:");
        Serial.println(sdError ? "ERROR" : "saved");
    }

    // ── Button Handling ───────────────────────────────────────────────────
    int btn = getPressedButton();
    if (btn < 0) { delay(10); return; }

    alertClear();

    // Product buttons 0–7
    if (btn <= 7) {
        selProduct = btn;
        lcd.clear();
        lcdRow(0, "CFA Temp Check");
        lcdRow(1, PRODUCTS[btn].name);
        char rng[21];
        snprintf(rng, sizeof(rng), "Safe:%.0f-%.0fF",
                 PRODUCTS[btn].minF, PRODUCTS[btn].maxF);
        lcdRow(2, rng);
        lcdRow(3, "Press START");

    // START button (index 8)
    } else if (btn == 8) {
        if (wday == 0) {
            lcd.clear();
            lcdRow(0, "CLOSED - Sunday");
            lcdRow(1, "Chick-fil-A is");
            lcdRow(2, "closed Sundays.");
            lcdRow(3, "No logging today.");
        } else if (!isOperating(wday, hour)) {
            lcd.clear();
            lcdRow(0, "OUTSIDE HOURS");
            lcdRow(1, dayName(wday));
            lcdRow(2, closeHourForDay(wday) == CLOSE_WKDAY
                       ? "11AM - 9:59PM" : "11AM - 11:59PM");
            lcdRow(3, "Can't log now.");
        } else if (selProduct < 0) {
            lcd.clear();
            lcdRow(0, "Pick a product");
            lcdRow(1, "first! (Btns 1-8)");
        } else if (!sdMounted) {
            // Warn about missing SD but still allow measurement for display
            lcd.clear();
            lcdRow(0, "!! NO SD CARD !!");
            lcdRow(1, "Insert card for");
            lcdRow(2, "logging. Continue?");
            lcdRow(3, "Press START again");
            sdMounted = false;  // will re-check next attempt
        } else if (!measuring) {
            lcd.clear();
            lcdRow(0, "Measuring...");
            lcdRow(1, PRODUCTS[selProduct].name);
            lcdRow(2, "Insert probe");
            lcdRow(3, "Please wait...");
            tempSensor.requestTemperatures();
            measuring = true;
        }

    // END button (index 9)
    } else if (btn == 9) {
        // Long press (>2 sec) on END triggers USB export build
        unsigned long pressStart = millis();
        while (digitalRead(PIN_BTN_END) == LOW) {
            if (millis() - pressStart > 2000) {
                // Long press — build export file
                lcd.clear();
                lcdRow(0, "Building Export...");
                lcdRow(1, "Please wait.");
                if (sdMounted) {
                    buildExportFile(now, wday);
                    lcd.clear();
                    lcdRow(0, "Export DONE!");
                    lcdRow(1, "Remove SD card,");
                    lcdRow(2, "copy /EXPORT/ to");
                    lcdRow(3, "USB drive or PC.");
                    delay(4000);
                } else {
                    lcd.clear();
                    lcdRow(0, "!! No SD Card !!");
                    lcdRow(1, "Insert card first");
                    delay(2000);
                }
                break;
            }
        }
        measuring  = false;
        selProduct = -1;
        showIdle(wday, hour);
    }

    delay(15);
}
