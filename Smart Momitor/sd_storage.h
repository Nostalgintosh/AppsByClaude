#pragma once

// ═══════════════════════════════════════════════════════════════════════════
//  sd_storage.h  —  MicroSD Card Storage Manager
//  Cyphamometer — CFA Food Safety Thermometer
//
// ┌─────────────────────────────────────────────────────────────────────────┐
// │                   WHY A MICROSD CARD?                                   │
// ├─────────────────────────────────────────────────────────────────────────┤
// │                                                                         │
// │  1. WIRELESS-STYLE PORTABILITY (Physical "Sneakernet" Transfer)         │
// │     The MicroSD card acts as a self-contained data shuttle. A team      │
// │     lead or food-safety manager can simply pop the card out of the      │
// │     thermometer, slide it into any computer's SD adapter or USB card    │
// │     reader, and instantly access all temperature logs — NO cables,      │
// │     NO Bluetooth pairing, NO Wi-Fi passwords, NO app required.          │
// │     This is effectively wireless data transfer using physical media:    │
// │     remove card → walk to office → insert in reader → open Excel.       │
// │     The card can then be reinserted and the thermometer resumes         │
// │     logging without any interruption to operations.                     │
// │                                                                         │
// │  2. LONG-TERM ARCHIVAL STORAGE                                          │
// │     MicroSD cards retain data for 10+ years without power. Unlike       │
// │     internal MCU RAM (lost on reboot) or cloud services (require        │
// │     internet), the SD card keeps every temperature reading safe even    │
// │     if the thermometer loses power, is replaced, or is reset.           │
// │                                                                         │
// │  3. STORAGE CAPACITY vs. LOG RETENTION                                  │
// │                                                                         │
// │     Each temperature reading = ~160 bytes in CSV.                       │
// │     Per hour check (8 products × 1 reading) = ~1,280 bytes = ~1.25 KB   │
// │     Per operating day (11 slots M-Th / 13 slots F-Sa):                  │
// │       • Mon-Thu: 11 slots × 1.25 KB  ≈  14 KB/day                       │
// │       • Fri-Sat: 13 slots × 1.25 KB  ≈  16 KB/day                       │
// │     Weekly total (6 operating days avg):     ~86 KB/week                │
// │     Monthly total (4.3 weeks avg):           ~370 KB/month              │
// │                                                                         │
// │     CARD SIZE → APPROXIMATE RETENTION PERIOD:                           │
// │     ┌───────────┬──────────────┬────────────────────────────────────┐   │
// │     │ Card Size │  Raw Cap.    │  CFA Log Retention Estimate        │   │
// │     ├───────────┼──────────────┼────────────────────────────────────┤   │
// │     │  1 GB     │  ~950 MB     │  ~2,500 months  (far exceeds need) │   │
// │     │  2 GB     │  ~1,900 MB   │  A practical minimum; ~5+ years    │   │
// │     │  4 GB     │  ~3,800 MB   │  ~10+ years of daily logging       │   │
// │     │  8 GB     │  ~7,600 MB   │  ~20+ years — lifetime device use  │   │
// │     │ 16 GB     │  ~15,200 MB  │  Essentially unlimited for CFA log │   │
// │     │ 32 GB     │  ~30,400 MB  │  Multi-location archive capacity   │   │
// │     └───────────┴──────────────┴────────────────────────────────────┘   │
// │                                                                         │
// │     REAL-WORLD RECOMMENDATION:                                          │
// │       • Minimum: 2 GB  → stores ~5 years of daily logs                  │
// │       • Typical: 8 GB  → stores a full device lifetime with room        │
// │                          for monthly full-day summary exports           │
// │       • Maximum useful: 32 GB (filesystem limits on FAT32 cap cards     │
// │         at 32 GB for best Arduino/ESP32 SD library compatibility)       │
// │                                                                         │
// │     IMPORTANT: This system creates ONE new CSV file per operating day.  │
// │     Files are never deleted or overwritten — they accumulate and        │
// │     remain searchable by date. At ~14 KB per day, even a 1 GB card      │
// │     holds over 70,000 days (~190 years) of logs. In practice, 2-4 GB    │
// │     is the recommended minimum for 1-2 years of assured retention       │
// │     with space to spare for full summaries and backup copies.           │
// │                                                                         │
// │  4. FAULT TOLERANCE                                                     │
// │     Every reading is written and flushed to the SD card immediately     │
// │     after it is taken. If the thermometer loses power mid-shift, all    │
// │     readings up to that point are already safely on the card.           │
// │     There is no data loss window beyond the single reading in progress. │
// │                                                                         │
// │  5. HEALTH DEPARTMENT / AUDIT COMPLIANCE                                │
// │     Many health departments require 30-90 days of temperature records   │
// │     to be retained on-site and available for inspection. The SD card    │
// │     stores every log with date, time, product, temperature, pass/fail,  │
// │     and corrective action — in Excel-readable CSV format, ready to      │
// │     print or email the moment an inspector asks.                        │
// │                                                                         │
// │  FORMAT: FAT32 (universally compatible with Windows, Mac, Linux,        │
// │          Excel, and all SD card readers)                                │
// │                                                                         │
// │  FILE NAMING CONVENTION:                                                │
// │    /CFA_LOGS/YYYY/MM/CFA_YYYYMMDD_DayName.CSV                           │
// │    Example:  /CFA_LOGS/2025/03/CFA_20250321_Fri.CSV                     │
// │    Organized in year/month subfolders for easy navigation.              │
// └─────────────────────────────────────────────────────────────────────────┘
//
//  HARDWARE WIRING (SPI bus on ESP32):
//    MicroSD Module   ESP32 GPIO
//    ─────────────── ──────────
//    CS  (chip sel.)     5
//    MOSI (data in)     23
//    MISO (data out)    19
//    SCK  (clock)       18
//    VCC                3.3 V  (some modules need 5 V — check module specs)
//    GND                GND
//
//  RECOMMENDED CARDS:
//    • SanDisk Industrial MicroSD (rated for temperature extremes)
//    • Samsung Pro Endurance (designed for continuous write applications)
//    • Avoid: cheap off-brand cards — they fail unpredictably
//    • Speed class: Class 10 or higher (U1 minimum)
//    • Capacity: 2 GB – 32 GB  (FAT32 formatted)
// ═══════════════════════════════════════════════════════════════════════════

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <map>
#include "thermometer.h"
#include "timer.h"

// ─────────────────────────────────────────────
//  SD CARD STORAGE INFO
//  (used to display capacity on LCD and logs)
// ─────────────────────────────────────────────
struct SDCardInfo {
    bool     mounted        = false;
    uint32_t totalMB        = 0;      // total capacity in MB
    uint32_t usedMB         = 0;      // approximate used space in MB
    uint32_t freeMB         = 0;      // free space in MB
    uint32_t estimatedDays  = 0;      // estimated days of logging remaining
    std::string cardType    = "Unknown";  // "SD", "SDHC", "SDXC"

    // Estimated bytes per full operating day of CFA logs
    // 8 products × avg 12 slots × ~160 bytes = ~15,360 bytes = ~15 KB/day
    static constexpr uint32_t BYTES_PER_DAY = 15360;

    void computeEstimates() {
        freeMB = totalMB - usedMB;
        estimatedDays = (freeMB * 1024UL * 1024UL) / BYTES_PER_DAY;
    }

    std::string retentionString() const {
        if (!mounted) return "No SD Card";
        if (estimatedDays > 730)  return ">2 years remaining";
        if (estimatedDays > 365)  return ">1 year remaining";
        if (estimatedDays > 90)   return "3-12 months remaining";
        if (estimatedDays > 30)   return "1-3 months remaining";
        if (estimatedDays > 7)    return "~1 month remaining";
        if (estimatedDays > 1)    return "< 1 week — replace card!";
        return "CARD FULL — replace now!";
    }
};

// ─────────────────────────────────────────────
//  SD STORAGE MANAGER
//  Desktop/simulation implementation using
//  std::fstream. On embedded hardware, replace
//  fstream calls with SD.open() / File.print().
// ─────────────────────────────────────────────
class SDStorageManager {
public:
    // sdRootPath: root mount point
    //   Desktop:   "./sd_card/"
    //   Linux MCU: "/sd/"
    //   Arduino:   (use SD library directly — see .ino sketch)
    explicit SDStorageManager(const std::string& sdRootPath  = "./sd_card/",
                               const std::string& storeName   = "")  // Set your store name here
        : root_(sdRootPath)
        , storeName_(storeName)
    {}

    // ── Mount / Initialise ───────────────────
    bool mount() {
        ensureDir(root_);
        ensureDir(root_ + "CFA_LOGS/");
        info_.mounted = true;
        info_.cardType = "SDHC";

        // Desktop simulation: fake capacity based on folder
        info_.totalMB = 8192;   // simulate 8 GB card
        info_.usedMB  = estimateUsedMB();
        info_.computeEstimates();

        logEvent("SD card mounted. " + info_.retentionString());
        return true;
    }

    bool isMounted() const { return info_.mounted; }
    const SDCardInfo& cardInfo() const { return info_; }

    // ── Write a single temperature entry ─────
    //  Called immediately after every measurement.
    //  Writes to both:
    //    1. Daily detail CSV  (one row per reading)
    //    2. Monthly summary   (one row per hour-slot)
    bool writeEntry(const TempLogEntry& e) {
        if (!info_.mounted) return false;

        ensureDayFile(e.timestamp);
        bool ok = appendToDayFile(e);

        // Also write to the running monthly index file
        appendToMonthIndex(e);

        return ok;
    }

    // ── Write hourly summary marker ──────────
    //  Called at the top of each new hour.
    bool writeHourlyMarker(int wday, int hour, int checks, int pass) {
        if (!info_.mounted) return false;

        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        std::string fn = dayFilePath(*t);

        std::ofstream f(fn, std::ios::app);
        if (!f.is_open()) return false;

        char buf[120];
        std::snprintf(buf, sizeof(buf),
            "# HOUR COMPLETE,%s,%02d:00,%s,%d,%d,%.0f%%",
            CFASchedule::dayName(wday).c_str(),
            hour,
            formatHour(hour).c_str(),
            checks, pass,
            checks > 0 ? 100.0f * pass / checks : 0.0f);
        f << buf << "\n";
        f.close();
        return true;
    }

    // ── Write end-of-day summary ─────────────
    bool writeDaySummary(const std::vector<TempLogEntry>& logs, int wday) {
        if (!info_.mounted) return false;

        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        std::string fn = dayFilePath(*t);

        std::ofstream f(fn, std::ios::app);
        if (!f.is_open()) return false;

        int pass = 0, fail = 0;
        for (const auto& e : logs) {
            if (e.inSafeRange) ++pass; else ++fail;
        }

        f << "\n# ═══════════════════════════════════\n";
        f << "# END OF DAY SUMMARY\n";
        f << "# Store:,"      << storeName_                        << "\n";
        f << "# Date:,"       << formatDate(*t)                    << "\n";
        f << "# Day:,"        << CFASchedule::dayName(wday)        << "\n";
        f << "# Hours:,"      << CFASchedule::hoursStr(wday)       << "\n";
        f << "# Total Checks:," << logs.size()                     << "\n";
        f << "# PASS:,"       << pass                              << "\n";
        f << "# FAIL:,"       << fail                              << "\n";
        if (!logs.empty()) {
            float pct = 100.0f * pass / (float)logs.size();
            f << "# Compliance:," << std::fixed << std::setprecision(1)
              << pct << "%\n";
        }
        f << "# Manager Sign-off:,___________________\n";
        f << "# ═══════════════════════════════════\n";
        f.close();

        // Update monthly index
        appendDaySummaryToMonthIndex(logs, wday, *t);
        return true;
    }

    // ── Export full-day CSV for USB pickup ───
    //  Copies the day file to a /EXPORT/ folder with a clean header.
    //  Manager removes SD card, copies /EXPORT/ to USB drive or PC.
    bool exportForUSB(const std::vector<TempLogEntry>& logs, int wday) {
        if (!info_.mounted) return false;

        ensureDir(root_ + "EXPORT/");
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);

        char fname[64];
        const char* dayAbbr[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
        std::strftime(fname, sizeof(fname),
                      "CFA_TempLog_%Y-%m-%d_", t);
        std::string exportPath = root_ + "EXPORT/" +
                                 fname + dayAbbr[wday] + ".csv";

        std::ofstream f(exportPath);
        if (!f.is_open()) return false;

        // ── Rich Excel header ──
        f << "CHICK-FIL-A FOOD SAFETY TEMPERATURE LOG\n";
        f << "Store:,"    << storeName_                         << "\n";
        f << "Date:,"     << formatDate(*t)                     << "\n";
        f << "Day:,"      << CFASchedule::dayName(wday)         << "\n";
        f << "Hours:,"    << CFASchedule::hoursStr(wday)        << "\n";
        f << "SD Card:,"  << info_.cardType << " "
                          << info_.totalMB << " MB"             << "\n";
        f << "Retention:,"<< info_.retentionString()            << "\n";
        f << "Exported:," << formatDateTime(*t)                 << "\n";
        f << "\n";

        // ── Hourly summary grid ──
        writeHourlyGrid(f, logs, wday);

        // ── Detail rows ──
        f << "DETAILED READINGS\n";
        f << "Date,Day,Hour Slot,Time,CFA Item,Temp (F),"
             "Min (F),Max (F),Status,Corrective Action\n";

        for (const auto& e : logs) {
            char timeBuf[16];
            std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &e.timestamp);
            f << formatDate(e.timestamp)                         << ","
              << CFASchedule::dayName(e.weekday)                 << ","
              << formatHour(e.hourSlot)                          << ","
              << timeBuf                                         << ","
              << csvEsc(e.cfaLabel)                             << ","
              << std::fixed << std::setprecision(1)
              << e.temperature                                   << ","
              << productMin(e.product)                           << ","
              << productMax(e.product)                           << ","
              << e.status                                        << ","
              << csvEsc(e.corrective)                           << "\n";
        }
        f << "\n";

        // ── Totals ──
        int pass = 0, fail = 0;
        for (const auto& e : logs) { if (e.inSafeRange) ++pass; else ++fail; }
        f << "DAILY TOTALS\n";
        f << "Total Checks:," << logs.size() << "\n";
        f << "PASS:,"         << pass        << "\n";
        f << "FAIL:,"         << fail        << "\n";
        if (!logs.empty()) {
            float pct = 100.0f * pass / (float)logs.size();
            f << "Compliance %:,"
              << std::fixed << std::setprecision(1) << pct << "\n";
        }
        f << "Manager Initials:,___\n";
        f << "Reviewed By:,___\n";
        f << "\n";
        f << "SD CARD INFO\n";
        f << "Card Type:,"       << info_.cardType        << "\n";
        f << "Total Space:,"     << info_.totalMB << " MB\n";
        f << "Used Space:,"      << info_.usedMB  << " MB\n";
        f << "Free Space:,"      << info_.freeMB  << " MB\n";
        f << "Estimated Remaining:," << info_.retentionString() << "\n";
        f.close();

        logEvent("Exported to USB: " + exportPath);
        return true;
    }

    // ── Log a system event (errors, mounts, etc.) ─
    void logEvent(const std::string& msg) {
        ensureDir(root_);
        std::ofstream f(root_ + "system_events.log", std::ios::app);
        if (!f.is_open()) return;
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        char ts[32];
        std::strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", t);
        f << "[" << ts << "] " << msg << "\n";
    }

private:
    std::string root_;
    std::string storeName_;
    SDCardInfo  info_;

    // ── Path builders ─────────────────────────
    //  /sd_card/CFA_LOGS/YYYY/MM/CFA_YYYYMMDD_Day.CSV
    std::string dayFilePath(const std::tm& t) const {
        char yearDir[8], monthDir[4];
        std::strftime(yearDir,  sizeof(yearDir),  "%Y", &t);
        std::strftime(monthDir, sizeof(monthDir), "%m", &t);

        std::string dir = root_ + "CFA_LOGS/" +
                          yearDir + "/" + monthDir + "/";
        ensureDir(dir);

        const char* dayAbbr[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
        char fname[40];
        std::strftime(fname, sizeof(fname), "CFA_%Y%m%d_", &t);
        return dir + fname + dayAbbr[t.tm_wday] + ".CSV";
    }

    std::string monthIndexPath(const std::tm& t) const {
        char yearDir[8], monthDir[4], monthStr[8];
        std::strftime(yearDir,  sizeof(yearDir),  "%Y", &t);
        std::strftime(monthDir, sizeof(monthDir), "%m", &t);
        std::strftime(monthStr, sizeof(monthStr), "%Y-%m", &t);

        std::string dir = root_ + "CFA_LOGS/" + yearDir + "/" + monthDir + "/";
        return dir + "INDEX_" + monthStr + ".CSV";
    }

    // ── File initialisation ───────────────────
    void ensureDayFile(const std::tm& ts) {
        std::string fn = dayFilePath(ts);
        if (fileExists(fn)) return;

        std::ofstream f(fn);
        if (!f.is_open()) return;

        // ── WHY MicroSD header written into each file ──
        f << "# CHICK-FIL-A FOOD SAFETY TEMPERATURE LOG\n";
        f << "# MicroSD Long-Term Storage File\n";
        f << "# This file is stored on the MicroSD card inside the thermometer.\n";
        f << "# To transfer: remove card, insert in USB SD reader, open in Excel.\n";
        f << "# Retention: card stores 1 month - 2+ years depending on card size.\n";
        f << "# Store:,"    << storeName_ << "\n";
        f << "# Card:,"     << info_.cardType << " " << info_.totalMB << " MB\n";
        f << "#\n";
        f << "Date,Day,Hour Slot,Time,CFA Item,Temp (F),"
             "Min (F),Max (F),Status,Corrective Action\n";
        f.close();
    }

    bool appendToDayFile(const TempLogEntry& e) {
        std::string fn = dayFilePath(e.timestamp);
        std::ofstream f(fn, std::ios::app);
        if (!f.is_open()) return false;

        char timeBuf[16];
        std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &e.timestamp);

        f << formatDate(e.timestamp)                          << ","
          << CFASchedule::dayName(e.weekday)                  << ","
          << formatHour(e.hourSlot)                           << ","
          << timeBuf                                          << ","
          << csvEsc(e.cfaLabel)                              << ","
          << std::fixed << std::setprecision(1)
          << e.temperature                                    << ","
          << productMin(e.product)                            << ","
          << productMax(e.product)                            << ","
          << e.status                                         << ","
          << csvEsc(e.corrective)                            << "\n";
        f.close();
        return true;
    }

    void appendToMonthIndex(const TempLogEntry& e) {
        std::string fn = monthIndexPath(e.timestamp);
        bool isNew = !fileExists(fn);
        std::ofstream f(fn, std::ios::app);
        if (!f.is_open()) return;
        if (isNew) {
            f << "# CFA Monthly Temperature Index\n";
            f << "# Auto-generated by thermometer SD storage system\n";
            f << "Date,Day,Hour,Product,Temp(F),Status\n";
        }
        char timeBuf[10];
        std::strftime(timeBuf, sizeof(timeBuf), "%H:%M", &e.timestamp);
        f << formatDate(e.timestamp)           << ","
          << CFASchedule::dayName(e.weekday)   << ","
          << timeBuf                           << ","
          << csvEsc(e.cfaLabel)               << ","
          << std::fixed << std::setprecision(1)
          << e.temperature                     << ","
          << e.status                          << "\n";
    }

    void appendDaySummaryToMonthIndex(const std::vector<TempLogEntry>& logs,
                                       int wday, const std::tm& t)
    {
        std::string fn = monthIndexPath(t);
        std::ofstream f(fn, std::ios::app);
        if (!f.is_open()) return;
        int pass = 0, fail = 0;
        for (const auto& e : logs) { if (e.inSafeRange) ++pass; else ++fail; }
        char buf[100];
        std::snprintf(buf, sizeof(buf),
            "# DAY END %s checks=%d pass=%d fail=%d pct=%.1f%%",
            formatDate(t).c_str(),
            (int)logs.size(), pass, fail,
            logs.empty() ? 0.0f : 100.0f * pass / (float)logs.size());
        f << buf << "\n";
    }

    // ── Hourly grid for export ────────────────
    void writeHourlyGrid(std::ofstream& f,
                         const std::vector<TempLogEntry>& logs,
                         int wday) const
    {
        int open  = CFASchedule::OPEN_HOUR;
        int close = CFASchedule::closeHour(wday);
        if (close == 0) { f << "(Closed today)\n\n"; return; }

        f << "HOURLY SUMMARY GRID\n";
        f << "Hour";
        for (const auto& p : PRODUCT_TABLE)
            f << "," << csvEsc(p.cfaLabel);
        f << ",All OK?\n";

        std::map<int, std::map<ProductID, const TempLogEntry*>> grid;
        for (const auto& e : logs)
            grid[e.hourSlot][e.product] = &e;

        for (int h = open; h < close; ++h) {
            f << formatHour(h);
            bool rowPass = true;
            auto& hm = grid[h];
            for (const auto& p : PRODUCT_TABLE) {
                auto it = hm.find(p.id);
                if (it == hm.end()) { f << ",--"; rowPass = false; }
                else {
                    f << "," << std::fixed << std::setprecision(1)
                      << it->second->temperature
                      << "(" << it->second->status << ")";
                    if (!it->second->inSafeRange) rowPass = false;
                }
            }
            f << "," << (rowPass ? "YES" : "NO") << "\n";
        }
        f << "\n";
    }

    // ── Estimate used space from file sizes ───
    uint32_t estimateUsedMB() const {
        // On desktop simulation, return a plausible small number
        return 12; // 12 MB "used" — realistic for a few months of logs
    }

    // ── Utility helpers ───────────────────────
    std::string formatDate(const std::tm& t) const {
        char buf[16];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
        return buf;
    }
    std::string formatDateTime(const std::tm& t) const {
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
        return buf;
    }
    float productMin(ProductID id) const {
        for (const auto& p : PRODUCT_TABLE) if (p.id == id) return p.minSafeTemp;
        return 0;
    }
    float productMax(ProductID id) const {
        for (const auto& p : PRODUCT_TABLE) if (p.id == id) return p.maxSafeTemp;
        return 0;
    }
    std::string csvEsc(const std::string& s) const {
        if (s.find_first_of(",\"") == std::string::npos) return s;
        std::string out = "\"";
        for (char c : s) { if (c == '"') out += '"'; out += c; }
        out += '"';
        return out;
    }
    bool fileExists(const std::string& p) const {
        std::ifstream f(p); return f.good();
    }
    void ensureDir(const std::string& path) const {
#ifdef __unix__
        ::system(("mkdir -p \"" + path + "\"").c_str());
#elif defined(_WIN32)
        ::system(("mkdir \"" + path + "\" 2>nul").c_str());
#endif
    }
};
