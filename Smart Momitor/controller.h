#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <cstdio>

#include "thermometer.h"
#include "sensor.h"
#include "timer.h"
#include "display.h"
#include "sd_storage.h"

// ═══════════════════════════════════════════════════════════════════════════
//  controller.h  —  Chick-fil-A Thermometer State Machine
//
//  Storage architecture:
//    ┌──────────────────────────────────────────────────────────────────┐
//    │  MicroSD Card  (primary long-term storage)                       │
//    │    • Written immediately on every reading — no data loss risk    │
//    │    • Organized /CFA_LOGS/YYYY/MM/CFA_YYYYMMDD_Day.CSV            │
//    │    • Monthly index file for quick lookup across days             │
//    │    • /EXPORT/ folder populated when manager presses USB export   │
//    │    • Manager removes card → inserts in USB reader → opens Excel  │
//    │    • 2 GB card ≈ 5 years; 8 GB card ≈ lifetime device use        │
//    └──────────────────────────────────────────────────────────────────┘
//
//  Workflow per reading:
//    1. Team member presses product button (1–8)
//    2. LCD shows product name + safe temperature range
//    3. Team member presses START and inserts probe into food
//    4. Sensor reads; green LED/PASS or red LED+buzzer/FAIL shown
//    5. Entry IMMEDIATELY written and flushed to MicroSD
//    6. Press END to clear and return to idle screen
//
//  Hourly:
//    At the top of each operating hour:
//      - LCD shows "NEW HOUR" reminder with slots remaining
//      - Hour-complete marker appended to SD card log
// ═══════════════════════════════════════════════════════════════════════════

class CFAThermometerController {
public:
    CFAThermometerController(TemperatureSensor* sensor,
                              Display*           display,
                              std::string        storeName = "",  // Set your store name
                              std::string        sdPath    = "./sd_card/")
        : sensor_(sensor)
        , display_(display)
        , storeName_(std::move(storeName))
        , sd_(sdPath, storeName_)
        , state_(ThermometerState::IDLE)
        , selectedProduct_(ProductID::NONE)
        , hourChecks_(0)
        , hourPass_(0)
    {
        timer_.onHourReset([this](int wday, int hour) {
            onHourReset(wday, hour);
        });
    }

    // ── Initialization ───────────────────────
    void begin() {
        sensor_->init();

        // Mount MicroSD — this is the primary storage system
        bool sdOk = sd_.mount();

        showIdle();

        std::cout
            << "╔════════════════════════════════════════════╗\n"
            << "║   Cyphamometer Food Safety Thermometer     ║\n"
            << "║   Store: " << storeName_ <<               "║\n"
            << "╠════════════════════════════════════════════╣\n"
            << "║   Schedule:                                ║\n"
            << "║     Mon–Thu  11:00 AM – 9:59 PM            ║\n"
            << "║     Fri–Sat  11:00 AM – 11:59 PM           ║\n"
            << "║     Sunday   CLOSED                        ║\n"
            << "╠════════════════════════════════════════════╣\n"
            << "║   MicroSD Storage:                         ║\n";

        if (sdOk) {
            const auto& info = sd_.cardInfo();
            std::cout
                << "║     Status:    MOUNTED ✓                     ║\n"
                << "║     Card:      " << info.cardType
                                       << "  " << info.totalMB << " MB\n"
                << "║     Free:      " << info.freeMB << " MB\n"
                << "║     Retention: " << info.retentionString() << "\n"
                << "║     Path:      /CFA_LOGS/YYYY/MM/CFA_*.CSV   ║\n";
        } else {
            std::cout
                << "║     Status:    ** NOT MOUNTED — CHECK CARD **║\n";
        }
        std::cout << "╚════════════════════════════════════════════╝\n";
    }

    // ── Main loop ────────────────────────────
    void loop() {
        timer_.tick();
        if (state_ == ThermometerState::MEASURING && sensor_->isReady()) {
            float temp = sensor_->readFahrenheit();
            finalizeMeasurement(temp);
        }
    }

    // ── Button events ─────────────────────────
    void onButtonPress(ButtonID btn) {
        switch (btn) {
            case ButtonID::BTN_REGULAR:  selectProduct(ProductID::REGULAR);  break;
            case ButtonID::BTN_SPICY:    selectProduct(ProductID::SPICY);    break;
            case ButtonID::BTN_GRILLED:  selectProduct(ProductID::GRILLED);  break;
            case ButtonID::BTN_CJ:       selectProduct(ProductID::CJ);       break;
            case ButtonID::BTN_PJ_AMERO: selectProduct(ProductID::PJ_AMERO); break;
            case ButtonID::BTN_LETTUCE:  selectProduct(ProductID::LETTUCE);  break;
            case ButtonID::BTN_TOMATOES: selectProduct(ProductID::TOMATOES); break;
            case ButtonID::BTN_FRIES:    selectProduct(ProductID::FRIES);    break;
            case ButtonID::BTN_START:    onStart(); break;
            case ButtonID::BTN_END:      onEnd();   break;
            default: break;
        }
    }

    // ── Export to USB (manager action) ───────
    //  Builds a clean Excel-ready CSV in /sd_card/EXPORT/
    //  Manager then removes card and transfers to office PC.
    void exportToUSB() {
        display_->clear();
        display_->print(0, 0, "Building Export...");
        display_->print(1, 0, storeName_);

        char countBuf[21];
        std::snprintf(countBuf, sizeof(countBuf), "%d records", (int)logs_.size());
        display_->print(2, 0, countBuf);
        display_->print(3, 0, "Please wait...");

        std::cout << "[SD Export] Writing " << logs_.size()
                  << " records to /EXPORT/ folder...\n";

        int wday = timer_.currentWday();

        // Write full-day summary to daily file
        sd_.writeDaySummary(logs_, wday);

        // Build the clean USB-transfer export file
        bool ok = sd_.exportForUSB(logs_, wday);

        if (ok) {
            display_->clear();
            display_->print(0, 0, "Export COMPLETE!");
            display_->print(1, 0, "Remove SD card,");
            display_->print(2, 0, "copy /EXPORT/ to");
            display_->print(3, 0, "your USB drive.");

            const auto& info = sd_.cardInfo();
            std::cout << "[SD Export] Done. Files in /sd_card/EXPORT/\n"
                      << "[SD Export] SD free: " << info.freeMB << " MB — "
                      << info.retentionString() << "\n";
        } else {
            display_->clear();
            display_->print(0, 0, "!! Export FAILED !!");
            display_->print(1, 0, "Check SD card.");
            display_->print(2, 0, "Is it inserted?");
            std::cout << "[SD Export] ERROR: Could not write to SD card.\n";
        }

        // Short pause, then back to idle
        showIdle();
    }

    // ── Display SD card status ─────────────────
    void showSDStatus() {
        display_->clear();
        const auto& info = sd_.cardInfo();
        display_->print(0, 0, "MicroSD Status:");

        if (!info.mounted) {
            display_->print(1, 0, "NOT MOUNTED");
            display_->print(2, 0, "Check card slot.");
            return;
        }

        char line[21];
        std::snprintf(line, sizeof(line), "%s  %u MB", info.cardType.c_str(), info.totalMB);
        display_->print(1, 0, line);

        std::snprintf(line, sizeof(line), "Free: %u MB", info.freeMB);
        display_->print(2, 0, line);

        // Truncate retention string to 20 chars for LCD
        std::string ret = info.retentionString();
        if (ret.size() > 20) ret = ret.substr(0, 20);
        display_->print(3, 0, ret);
    }

    // ── Accessors ─────────────────────────────
    int              logCount()  const { return (int)logs_.size(); }
    bool             isOpen()    const { return timer_.isOpen();   }
    const SDCardInfo& sdInfo()   const { return sd_.cardInfo();    }

private:
    TemperatureSensor*        sensor_;
    Display*                  display_;
    std::string               storeName_;
    SDStorageManager          sd_;
    CFAHourlyTimer            timer_;

    ThermometerState          state_;
    ProductID                 selectedProduct_;
    std::vector<TempLogEntry> logs_;
    std::vector<TempLogEntry> currentHourLogs_;
    int                       hourChecks_;
    int                       hourPass_;

    // ── SELECT PRODUCT ────────────────────────
    void selectProduct(ProductID id) {
        selectedProduct_ = id;
        state_ = ThermometerState::PRODUCT_SELECTED;

        const Product* p = findProduct(id);
        display_->clear();
        display_->print(0, 0, "Cyphamometer");
        display_->print(1, 0, p ? p->name : "???");
        if (p) {
            char range[21];
            std::snprintf(range, sizeof(range), "Safe: %.0f-%.0fF",
                          p->minSafeTemp, p->maxSafeTemp);
            display_->print(2, 0, range);
        }
        display_->print(3, 0, "START=Read END=Clr");
        std::cout << "[Select] " << (p ? p->cfaLabel : "?") << "\n";
    }

    // ── START BUTTON ──────────────────────────
    void onStart() {
        int wday = timer_.currentWday();
        int hour = timer_.currentHour();

        if (wday == 0) {
            display_->clear();
            display_->print(0, 0, "CLOSED - Sunday");
            display_->print(1, 0, "Chick-fil-A is");
            display_->print(2, 0, "closed Sundays.");
            std::cout << "[Start] Blocked — Sunday.\n";
            return;
        }
        if (!CFASchedule::isOperating(wday, hour)) {
            display_->clear();
            display_->print(0, 0, "OUTSIDE HOURS");
            display_->print(1, 0, CFASchedule::dayName(wday));
            display_->print(2, 0, CFASchedule::hoursStr(wday));
            display_->print(3, 0, "Cannot log now.");
            std::cout << "[Start] Blocked — outside CFA hours.\n";
            return;
        }
        if (selectedProduct_ == ProductID::NONE) {
            display_->clear();
            display_->print(0, 0, "Select a product");
            display_->print(1, 0, "first! (Btns 1-8)");
            return;
        }

        state_ = ThermometerState::MEASURING;
        const Product* p = findProduct(selectedProduct_);
        display_->clear();
        display_->print(0, 0, "Measuring...");
        display_->print(1, 0, p ? p->name : "---");
        display_->print(2, 0, "Insert probe now");
        display_->print(3, 0, "Please wait...");

        sensor_->startMeasurement();
        std::cout << "[Start] Measuring " << (p ? p->cfaLabel : "?") << "...\n";
    }

    // ── END BUTTON ────────────────────────────
    void onEnd() {
        if (state_ == ThermometerState::MEASURING) {
            state_ = ThermometerState::IDLE;
            selectedProduct_ = ProductID::NONE;
            std::cout << "[End] Measurement cancelled.\n";
        } else {
            selectedProduct_ = ProductID::NONE;
        }
        showIdle();
    }

    // ── FINALIZE MEASUREMENT ──────────────────
    void finalizeMeasurement(float tempF) {
        const Product* p = findProduct(selectedProduct_);
        if (!p) { state_ = ThermometerState::IDLE; showIdle(); return; }

        bool safe = (tempF >= p->minSafeTemp && tempF <= p->maxSafeTemp);

        std::string status;
        if (safe) {
            status = "PASS";
        } else if (tempF < p->minSafeTemp) {
            status = p->isHot ? "FAIL-COLD" : "FAIL-WARM";
        } else {
            status = "FAIL-HOT";
        }

        std::string corrective;
        if (!safe) {
            if (p->isHot && tempF < p->minSafeTemp)
                corrective = "Reheat or discard";
            else if (p->isHot && tempF > p->maxSafeTemp)
                corrective = "Check holding unit";
            else if (!p->isHot && tempF > p->maxSafeTemp)
                corrective = "Move to cooler/discard";
            else
                corrective = "Check refrigeration";
        }

        std::tm now = getCurrentTm();
        TempLogEntry entry;
        entry.timestamp   = now;
        entry.weekday     = now.tm_wday;
        entry.hourSlot    = now.tm_hour;
        entry.product     = selectedProduct_;
        entry.productName = p->name;
        entry.cfaLabel    = p->cfaLabel;
        entry.temperature = tempF;
        entry.inSafeRange = safe;
        entry.status      = status;
        entry.corrective  = corrective;

        logs_.push_back(entry);
        currentHourLogs_.push_back(entry);
        ++hourChecks_;
        if (safe) ++hourPass_;

        // ── Write to MicroSD immediately ──────
        //  This is the core of the long-term storage system.
        //  Each entry is flushed to the card the moment it's taken.
        //  Even a sudden power loss cannot erase already-written data.
        bool sdOk = sd_.writeEntry(entry);
        if (!sdOk) {
            std::cout << "[SD] WARNING: Could not write to MicroSD card!\n";
        }

        // ── Update display ────────────────────
        state_ = ThermometerState::RESULT_DISPLAY;
        display_->clear();
        display_->print(0, 0, p->name);

        char line1[21], line2[21];
        std::snprintf(line1, sizeof(line1), "%.1fF  [%s]", tempF, status.c_str());
        display_->print(1, 0, line1);

        std::snprintf(line2, sizeof(line2), "OK: %.0f-%.0fF",
                      p->minSafeTemp, p->maxSafeTemp);
        display_->print(2, 0, line2);

        // Show SD status on bottom row
        const auto& info = sd_.cardInfo();
        char sdLine[21];
        if (info.mounted) {
            std::snprintf(sdLine, sizeof(sdLine), "SD OK | END to cont.");
        } else {
            std::snprintf(sdLine, sizeof(sdLine), "SD ERR| END to cont.");
        }
        display_->print(3, 0, sdLine);

        display_->showAlert(!safe);

        std::cout << "[Result] " << p->cfaLabel
                  << "  " << tempF << "°F"
                  << "  [" << status << "]"
                  << "  SD:" << (sdOk ? "saved" : "ERROR");
        if (!corrective.empty())
            std::cout << "  => " << corrective;
        std::cout << "\n";
    }

    // ── HOURLY RESET ──────────────────────────
    void onHourReset(int wday, int newHour) {
        std::cout << "[Hour Reset] " << CFASchedule::dayName(wday)
                  << " " << formatHour(newHour)
                  << "  (checks=" << hourChecks_
                  << " pass=" << hourPass_ << ")\n";

        // Write hour-complete marker to SD card
        sd_.writeHourlyMarker(wday, newHour, hourChecks_, hourPass_);

        // LCD notification
        display_->clear();
        display_->print(0, 0, "-- NEW HOUR --");
        display_->print(1, 0, CFASchedule::dayName(wday));
        display_->print(2, 0, formatHour(newHour));

        char buf[21];
        int remaining = CFASchedule::closeHour(wday) - newHour - 1;
        std::snprintf(buf, sizeof(buf), "%d slots remain", remaining);
        display_->print(3, 0, buf);

        // Reset per-hour counters
        currentHourLogs_.clear();
        hourChecks_ = 0;
        hourPass_   = 0;

        showIdle();
    }

    // ── IDLE SCREEN ───────────────────────────
    void showIdle() {
        state_ = ThermometerState::IDLE;
        display_->clear();

        int wday = timer_.currentWday();
        int hour = timer_.currentHour();

        display_->print(0, 0, "Cyphamometer");

        if (wday == 0) {
            display_->print(1, 0, "Closed (Sunday)");
            display_->print(2, 0, "No logging today.");
        } else if (CFASchedule::isOperating(wday, hour)) {
            display_->print(1, 0, CFASchedule::dayName(wday));
            display_->print(2, 0, "Pick product 1-8");
            display_->print(3, 0, "then press START");
        } else {
            display_->print(1, 0, CFASchedule::dayName(wday));
            display_->print(2, 0, CFASchedule::hoursStr(wday));
            display_->print(3, 0, "Outside hours.");
        }
    }

    const Product* findProduct(ProductID id) const {
        for (const auto& p : PRODUCT_TABLE)
            if (p.id == id) return &p;
        return nullptr;
    }
};
