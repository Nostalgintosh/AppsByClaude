# 🌡️ Cyphamometer
## CFA Food Safety Thermometer — Complete C++ Embedded System

The **Cyphamometer** is a custom ESP32-based food safety thermometer built for Chick-fil-A operations. It enforces the CFA operating schedule, checks 8 product temperature ranges, logs every reading to a MicroSD card, and exports Excel-ready CSV files for health department compliance.

---

## 🏷️ Store Name Configuration

The store/owner name is **configurable**. Set it in one place before uploading:

**In `cfa_thermometer.ino` (hardware):**
```cpp
const char* STORE_NAME = "";  // ← Set your store name, e.g. "CFA #03745"
```

**In `main.cpp` (desktop simulation):**
```cpp
std::string storeName = "";  // ← Set your store name, e.g. "CFA #03745"
```

The name appears in every CSV log file header, on the LCD boot screen, and in exported reports.

---

## 📦 Why a MicroSD Card?

The MicroSD card is the **physical wireless drive** of the Cyphamometer. No Wi-Fi, no Bluetooth, no app needed:

1. Press the EXPORT button on the Cyphamometer
2. Slide the MicroSD card out of the slot
3. Walk to the office — insert into any USB card reader on any PC
4. Open the `/EXPORT/` folder — files open directly in Microsoft Excel
5. Slide the card back in — Cyphamometer resumes logging instantly

**Storage capacity at full CFA daily logging pace:**

| Card | Retention |
|------|-----------|
| 2 GB | ~5 years |
| 4 GB | ~10 years |
| 8 GB | ~20 years (full device lifetime) |

---

## 🕐 CFA Operating Schedule

| Day | Open | Close | Check Slots |
|-----|------|-------|-------------|
| Monday | 11:00 AM | 9:59 PM | 11 AM – 9 PM (11 slots) |
| Tuesday | 11:00 AM | 9:59 PM | 11 AM – 9 PM |
| Wednesday | 11:00 AM | 9:59 PM | 11 AM – 9 PM |
| Thursday | 11:00 AM | 9:59 PM | 11 AM – 9 PM |
| Friday | 11:00 AM | 11:59 PM | 11 AM – 11 PM (13 slots) |
| Saturday | 11:00 AM | 11:59 PM | 11 AM – 11 PM |
| **Sunday** | — | — | **CLOSED — logging blocked** |

---

## 🍗 Button Map & Safe Temperature Ranges

| Button | CFA Product | Min °F | Max °F | Hold Type |
|--------|-------------|--------|--------|-----------|
| 1 | Original Chicken Sandwich | 140 | 180 | Hot hold |
| 2 | Spicy Deluxe | 140 | 180 | Hot hold |
| 3 | Grilled Chicken Filet | 140 | 180 | Hot hold |
| 4 | Nuggets / Chick-n-Strips | 140 | 180 | Hot hold |
| 5 | Mac & Cheese / Soup | 140 | 180 | Hot hold |
| 6 | Lettuce / Cold Toppings | 34 | 41 | Cold hold |
| 7 | Tomatoes / Produce | 34 | 41 | Cold hold |
| 8 | Waffle Fries | 135 | 180 | Hot hold |
| START | Begin measurement | | | |
| END | Confirm / reset (hold 2 sec = SD export) | | | |

---

## ⚙️ ESP32 Hardware Wiring

```
DS18B20 PROBE
  DATA  →  GPIO 4  (+ 4.7 kΩ pullup to 3.3V)

LCD 20×4 (I2C)
  SDA  →  GPIO 21
  SCL  →  GPIO 22

MICROSD (SPI)
  CS   →  GPIO 5
  MOSI →  GPIO 23
  MISO →  GPIO 19
  SCK  →  GPIO 18

DS3231 RTC (shared I2C)
  SDA  →  GPIO 21
  SCL  →  GPIO 22

LEDs + BUZZER
  Green LED  →  GPIO 25  →  220 Ω  →  GND
  Red LED    →  GPIO 26  →  220 Ω  →  GND
  Buzzer     →  GPIO 27  →  GND

BUTTONS (INPUT_PULLUP → GND)
  GPIO 13  BTN 1  Original CFS
  GPIO 14  BTN 2  Spicy Deluxe
  GPIO 15  BTN 3  Grilled Filet
  GPIO 16  BTN 4  Nuggets/Strips
  GPIO 17  BTN 5  Mac & Cheese
  GPIO 18  BTN 6  Lettuce
  GPIO 19  BTN 7  Tomatoes
  GPIO 23  BTN 8  Waffle Fries
  GPIO 32  BTN START
  GPIO 33  BTN END
```

---

## 📁 Source Files

| File | Purpose |
|------|---------|
| `thermometer.h` | CFA products, temp ranges, schedule constants |
| `sensor.h` | DS18B20 + SimulatedSensor abstraction |
| `timer.h` | Day-aware hourly timer (Mon–Thu vs Fri–Sat) |
| `display.h` | Console (desktop) and 20×4 I2C LCD (hardware) |
| `sd_storage.h` | MicroSD manager — write, organize, export |
| `controller.h` | Cyphamometer state machine — orchestrates all modules |
| `main.cpp` | Desktop simulation + interactive/demo mode |
| `cfa_thermometer.ino` | Arduino/ESP32 hardware sketch |
| `CMakeLists.txt` | CMake build for desktop testing |

---

## 💻 Desktop Build & Testing

```bash
# Build
g++ -std=c++17 -O2 -Wall -o cyphamometer main.cpp -pthread

# Interactive mode (simulates MicroSD as ./sd_card/ folder)
./cyphamometer

# Demo mode — all 8 products, generates real CSV files in ./sd_card/
./cyphamometer --demo
```

**Keyboard controls:**

| Key | Action |
|-----|--------|
| `1`–`8` | Select product |
| `s` | START measurement |
| `e` | END / reset |
| `u` | Build SD export file |
| `i` | Show MicroSD capacity & retention |
| `t` | Set simulated temperature |
| `q` | Quit (saves final export) |

---

## 📊 MicroSD File Structure

```
/sd_card/
  CFA_LOGS/YYYY/MM/CFA_YYYYMMDD_Day.CSV    ← one file per operating day
  CFA_LOGS/YYYY/MM/INDEX_YYYY-MM.CSV        ← monthly quick-reference index
  EXPORT/CFA_TempLog_YYYY-MM-DD_Day.csv     ← clean Excel export for manager
  system_events.log                          ← mount/error/export events
```

---

## 🔧 Customization

**Store name** — set once in `main.cpp` or `cfa_thermometer.ino` (see top of this file).

**Temperature ranges** — in `thermometer.h`, `PRODUCT_TABLE` array.

**Operating hours** — in `thermometer.h`, `CFASchedule` constants (`OPEN_HOUR`, `CLOSE_MTH`, `CLOSE_FS`).

**SD card path** — `"./sd_card/"` on desktop, `"/sd/"` on hardware.
