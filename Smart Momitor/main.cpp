// ═══════════════════════════════════════════════════════════════════════════
//  CYPHAMOMETER — CFA Food Safety Thermometer
//  main.cpp  —  Desktop simulation / interactive test harness
//
// ┌─────────────────────────────────────────────────────────────────────────┐
// │                  HOW THE MICROSD CARD WORKS HERE                        │
// ├─────────────────────────────────────────────────────────────────────────┤
// │                                                                         │
// │  On real hardware (ESP32 + MicroSD breakout), every temperature         │
// │  reading is written directly to the card the moment it's taken.         │
// │  This desktop build simulates that card as a folder: ./sd_card/         │
// │                                                                         │
// │  WHY A MICROSD CARD (not Wi-Fi, not USB cable, not Bluetooth)?          │
// │                                                                         │
// │  The MicroSD card IS the wireless transfer system. No network needed.   │
// │  A manager just:                                                        │
// │    1. Pops the card out of the thermometer (fingernail-sized)           │
// │    2. Slides it into a $5 USB card reader on any PC                     │
// │    3. Opens the CSV files directly in Microsoft Excel                   │
// │    4. Reinserts the card — device resumes logging instantly             │
// │                                                                         │
// │  STORAGE CAPACITY & RETENTION:                                          │
// │    Each reading ≈ 180 bytes. At full CFA daily logging pace:            │
// │      2 GB card  →  ~5 years of daily logs  (minimum recommended)        │
// │      4 GB card  →  ~10 years                                            │
// │      8 GB card  →  ~20 years  (ideal — device lifetime)                 │
// │    Cards as small as 1 GB store well over 100 years of this data.       │
// │    In practice, get a 4–8 GB Samsung Pro Endurance or SanDisk           │
// │    Industrial card — rated for continuous write workloads.              │
// │                                                                         │
// │  FOLDER STRUCTURE ON CARD:                                              │
// │    ./sd_card/                                                           │
// │      CFA_LOGS/YYYY/MM/CFA_YYYYMMDD_Day.CSV   ← one file per day         │
// │      CFA_LOGS/YYYY/MM/INDEX_YYYY-MM.CSV       ← monthly quick index     │
// │      EXPORT/CFA_TempLog_YYYY-MM-DD_Day.csv    ← clean Excel export      │
// │      system_events.log                         ← mount/error journal    │
// │                                                                         │
// │  On embedded hardware the root path is "/sd/" or "/sdcard/".            │
// │  The SDStorageManager class (sd_storage.h) handles all path logic.      │
// └─────────────────────────────────────────────────────────────────────────┘
//
//  Build:
//    g++ -std=c++17 -O2 -Wall -o cfa_therm main.cpp -pthread
//  Run interactive:
//    ./cfa_therm
//  Run automated demo (exercises all products, generates SD card files):
//    ./cfa_therm --demo
// ═══════════════════════════════════════════════════════════════════════════

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#include "thermometer.h"
#include "sensor.h"
#include "display.h"
#include "timer.h"
#include "sd_storage.h"
#include "controller.h"

// ─────────────────────────────────────────────
//  KEYBOARD MENU
// ─────────────────────────────────────────────
void printMenu(const CFAThermometerController& ctrl) {
    const auto& sd = ctrl.sdInfo();

    std::cout
        << "\n╔══════════════════════════════════════════════╗\n"
        << "  ║   CYPHAMOMETER — CFA Food Safety Thermometer ║\n"
        << "  ╠══════════════════════════════════════════════╣\n"
        << "  ║   [1] Original CFS     [2] Spicy Deluxe      ║\n"
        << "  ║   [3] Grilled Filet    [4] Nuggets/Strips    ║\n"
        << "  ║   [5] Mac & Cheese     [6] Lettuce           ║\n"
        << "  ║   [7] Tomatoes         [8] Waffle Fries      ║\n"
        << "  ╠══════════════════════════════════════════════╣\n"
        << "  ║   [s] START   [e] END   [u] SD Export        ║\n"
        << "  ║   [t] Set temp (sim)    [i] SD Info  [q] Quit║\n"
        << "  ╠══════════════════════════════════════════════╣\n"
        << "  ║  MicroSD: " << std::left << std::setw(34)
                          << (sd.mounted ? sd.retentionString() : "NOT MOUNTED")
        << "  ║\n"
        << "  ╚══════════════════════════════════════════════╝\n"
        << "Input: ";
}

ButtonID charToButton(char c) {
    switch (c) {
        case '1': return ButtonID::BTN_REGULAR;
        case '2': return ButtonID::BTN_SPICY;
        case '3': return ButtonID::BTN_GRILLED;
        case '4': return ButtonID::BTN_CJ;
        case '5': return ButtonID::BTN_PJ_AMERO;
        case '6': return ButtonID::BTN_LETTUCE;
        case '7': return ButtonID::BTN_TOMATOES;
        case '8': return ButtonID::BTN_FRIES;
        case 's': case 'S': return ButtonID::BTN_START;
        case 'e': case 'E': return ButtonID::BTN_END;
        default:  return ButtonID::BTN_NONE;
    }
}

// ─────────────────────────────────────────────
//  SD CARD INFO DISPLAY
// ─────────────────────────────────────────────
void printSDInfo(const CFAThermometerController& ctrl) {
    const auto& sd = ctrl.sdInfo();
    std::cout << "\n┌──────────────────────────────────────────────────┐\n"
              << "  │             MICROSD CARD STATUS                  │\n"
              << "  ├──────────────────────────────────────────────────┤\n";
    if (!sd.mounted) {
        std::cout
            << "│  Status  : NOT MOUNTED                               │\n"
            << "│  Action  : Check card is inserted correctly.         │\n";
    } else {
        std::cout
            << "│  Status  : MOUNTED ✓                                 │\n"
            << "│  Type    : " << std::left << std::setw(39) << sd.cardType       << "│\n"
            << "│  Total   : " << std::setw(39) << (std::to_string(sd.totalMB) + " MB") << "│\n"
            << "│  Free    : " << std::setw(39) << (std::to_string(sd.freeMB)  + " MB") << "│\n"
            << "│  Est. days left: " << std::setw(33) << sd.estimatedDays          << "│\n"
            << "│  Retention: " << std::setw(38) << sd.retentionString()           << "│\n";
    }
    std::cout
        << "├──────────────────────────────────────────────────┤\n"
        << "│  WHY MICROSD?                                    │\n"
        << "│  Pop card out → insert USB reader → open Excel.  │\n"
        << "│  No cable. No Wi-Fi. No app. Instant transfer.   │\n"
        << "│                                                  │\n"
        << "│  Retention by card size:                         │\n"
        << "│    2 GB  →  ~5 years of daily CFA logging        │\n"
        << "│    4 GB  →  ~10 years                            │\n"
        << "│    8 GB  →  ~20 years (full device lifetime)     │\n"
        << "│   32 GB  →  effectively unlimited                │\n"
        << "│                                                  │\n"
        << "│  Files: sd_card/CFA_LOGS/YYYY/MM/CFA_date.CSV    │\n"
        << "│  Export: sd_card/EXPORT/CFA_TempLog_date.CSV     │\n"
        << "└──────────────────────────────────────────────────┘\n";
}

// ─────────────────────────────────────────────
//  AUTOMATED DEMO
//  Exercises every product with pass/fail temps,
//  writes real files to ./sd_card/ folder tree.
// ─────────────────────────────────────────────
void runDemo(CFAThermometerController& ctrl, SimulatedSensor& sensor) {
    using ms = std::chrono::milliseconds;

    std::cout
        << "\n╔══════════════════════════════════════════╗\n"
        << "║       CYPHAMOMETER DEMO MODE         ║\n"
        << "║  Writing real CSV files to ./sd_card/   ║\n"
        << "╚══════════════════════════════════════════╝\n\n";

    struct TC {
        ButtonID    btn;
        const char* name;
        float       inject;
        bool        expectPass;
        const char* reason;
    };

    const TC tests[] = {
        { ButtonID::BTN_REGULAR,  "Original CFS",   165.0f, true,  "within 140-180°F" },
        { ButtonID::BTN_SPICY,    "Spicy Deluxe",   128.0f, false, "FAIL: below 140°F min" },
        { ButtonID::BTN_GRILLED,  "Grilled Filet",  155.0f, true,  "within 140-180°F" },
        { ButtonID::BTN_CJ,       "Nuggets/Strips", 145.0f, true,  "within 140-180°F" },
        { ButtonID::BTN_PJ_AMERO, "Mac & Cheese",   185.0f, false, "FAIL: above 180°F max" },
        { ButtonID::BTN_LETTUCE,  "Lettuce",         38.0f, true,  "within 34-41°F" },
        { ButtonID::BTN_TOMATOES, "Tomatoes",        44.0f, false, "FAIL: above 41°F max" },
        { ButtonID::BTN_FRIES,    "Waffle Fries",   148.0f, true,  "within 135-180°F" },
    };

    for (const auto& tc : tests) {
        std::cout << "  Testing: " << tc.name
                  << "  @ " << tc.inject << "°F"
                  << "  (" << tc.reason << ")\n";

        sensor.setBaseTemp(tc.inject);
        ctrl.onButtonPress(tc.btn);
        std::this_thread::sleep_for(ms(100));

        ctrl.onButtonPress(ButtonID::BTN_START);
        std::this_thread::sleep_for(ms(100));
        ctrl.loop();  // trigger sensor → finalizeMeasurement → SD write
        std::this_thread::sleep_for(ms(200));

        ctrl.onButtonPress(ButtonID::BTN_END);
        std::this_thread::sleep_for(ms(100));
    }

    std::cout << "\n[SD] Building export file...\n";
    ctrl.exportToUSB();

    std::cout << "\n[Demo] Complete!\n"
              << "  Records logged : " << ctrl.logCount() << "\n"
              << "  SD folder      : ./sd_card/\n"
              << "  Daily CSV      : ./sd_card/CFA_LOGS/YYYY/MM/CFA_date.CSV\n"
              << "  Export CSV     : ./sd_card/EXPORT/CFA_TempLog_date.CSV\n"
              << "  System log     : ./sd_card/system_events.log\n\n";

    printSDInfo(ctrl);
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main(int argc, char* argv[]) {
    std::srand((unsigned)std::time(nullptr));

    // ── Configuration — edit to match your store ──
    std::string storeName = "";  // ← Set your store name, e.g. "CFA #03745"
    std::string sdPath    = "./sd_card/";   // simulates MicroSD mount point
                                            // on hardware: "/sd/" or "/sdcard/"

    // ── Hardware abstraction layers ────────────
    //  Swap these for real hardware classes when deploying on ESP32:
    //    SimulatedSensor → DS18B20Sensor(GPIO_PIN)
    //    ConsoleDisplay  → LCD20x4Display(I2C_ADDR)
    SimulatedSensor sensor(160.0f, 1.5f);
    ConsoleDisplay  display;

    // ── Controller (owns MicroSD + timer + state machine) ──
    CFAThermometerController controller(&sensor, &display, storeName, sdPath);
    controller.begin();

    // ── Demo mode ──────────────────────────────
    if (argc > 1 && std::string(argv[1]) == "--demo") {
        runDemo(controller, sensor);
        return 0;
    }

    // ── Interactive mode ────────────────────────
    bool running = true;
    while (running) {
        controller.loop();
        printMenu(controller);

        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;
        char c = input[0];

        if (c == 'q' || c == 'Q') {
            running = false;

        } else if (c == 'u' || c == 'U') {
            // Export — builds clean Excel file in sd_card/EXPORT/
            // On hardware: manager removes card, copies /EXPORT/ to office PC
            controller.exportToUSB();

        } else if (c == 'i' || c == 'I') {
            printSDInfo(controller);

        } else if (c == 't' || c == 'T') {
            std::cout << "Simulated temperature (°F): ";
            float t; std::cin >> t; std::cin.ignore();
            sensor.setBaseTemp(t);
            std::cout << "  → Sensor base set to " << t << "°F\n";

        } else {
            ButtonID btn = charToButton(c);
            if (btn != ButtonID::BTN_NONE) {
                controller.onButtonPress(btn);
                if (c == 's' || c == 'S') {
                    std::this_thread::sleep_for(std::chrono::milliseconds(120));
                    controller.loop();  // complete measurement + SD write
                }
            } else {
                std::cout << "Unknown key. Use 1-8 | s=START | e=END | u=Export"
                          << " | i=SD Info | t=Temp | q=Quit\n";
            }
        }
    }

    // ── Final save on exit ─────────────────────
    std::cout << "\nSaving final export before exit ("
              << controller.logCount() << " records)...\n";
    controller.exportToUSB();
    std::cout << "Files saved to: " << sdPath << "\n";

    return 0;
}
