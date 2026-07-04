#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdio>
#include "thermometer.h"
#include "timer.h"

// ═══════════════════════════════════════════════════════════════════════════
//  excel_export.h
//
//  Writes a Chick-fil-A style food-safety temperature log to CSV.
//  The CSV opens directly in Microsoft Excel with:
//    • Restaurant header block (name, date, day, schedule)
//    • Hourly summary grid  (11 AM … 9 PM or 11 PM)
//    • Full detail rows for every individual reading
//    • Daily compliance totals
//
//  FILE NAMING:  CFA_TempLog_YYYY-MM-DD_DayName.csv
//  USB/SD path:  usbMountPath (e.g. "./usb_output/" or "/sd/")
// ═══════════════════════════════════════════════════════════════════════════

class ExcelExporter {
public:
    explicit ExcelExporter(const std::string& usbPath = "./usb_output/",
                           const std::string& storeName = "Chick-fil-A")
        : mountPath_(usbPath), storeName_(storeName) {}

    // ── Full-day export (call on USB connect or day-end) ─────────────────
    bool exportDayLog(const std::vector<TempLogEntry>& logs,
                      int wday)    // 0=Sun … 6=Sat
    {
        ensureDirectory(mountPath_);
        std::string fn = mountPath_ + buildFilename();
        std::ofstream f(fn);
        if (!f.is_open()) return false;

        writeStoreHeader(f, wday);
        writeHourlySummaryGrid(f, logs, wday);
        writeDetailSection(f, logs);
        writeTotalsBlock(f, logs);

        f.close();
        return true;
    }

    // ── Append a single reading immediately after it is taken ────────────
    //    Creates the file + header row if it doesn't yet exist.
    bool appendEntry(const TempLogEntry& e) {
        ensureDirectory(mountPath_);
        std::string fn = mountPath_ + buildFilename();
        bool newFile = !fileExists(fn);

        std::ofstream f(fn, std::ios::app);
        if (!f.is_open()) return false;

        if (newFile) {
            // Write a lightweight header so Excel shows columns on first open
            f << "Date,Day,Hour Slot,Time,CFA Item,Temp (F),"
                 "Min Safe (F),Max Safe (F),Status,Corrective Action\n";
        }

        char timeBuf[16];
        std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &e.timestamp);

        f << formatTmDate(e.timestamp)              << ","
          << CFASchedule::dayName(e.weekday)        << ","
          << formatHour(e.hourSlot)                 << ","
          << timeBuf                                << ","
          << csvEsc(e.cfaLabel)                    << ","
          << std::fixed << std::setprecision(1)
          << e.temperature                          << ","
          << productMin(e.product)                  << ","
          << productMax(e.product)                  << ","
          << e.status                               << ","
          << csvEsc(e.corrective)                  << "\n";

        return true;
    }

private:
    std::string mountPath_;
    std::string storeName_;

    // ── Header ──────────────────────────────────────────────────────────
    void writeStoreHeader(std::ofstream& f, int wday) {
        f << "CHICK-FIL-A FOOD SAFETY TEMPERATURE LOG\n";
        f << "Store:,"    << csvEsc(storeName_)                << "\n";
        f << "Date:,"     << currentDateStr()                  << "\n";
        f << "Day:,"      << CFASchedule::dayName(wday)        << "\n";
        f << "Hours:,"    << CFASchedule::hoursStr(wday)       << "\n";
        f << "Generated:,"<< currentDateTimeStr()              << "\n";
        f << "\n";
    }

    // ── Hourly summary grid ──────────────────────────────────────────────
    //  Rows = one per hourly check slot
    //  Cols = each of the 8 CFA products
    void writeHourlySummaryGrid(std::ofstream& f,
                                const std::vector<TempLogEntry>& logs,
                                int wday)
    {
        int open  = CFASchedule::OPEN_HOUR;
        int close = CFASchedule::closeHour(wday);
        if (close == 0) { f << "(Restaurant closed today)\n\n"; return; }

        f << "HOURLY TEMPERATURE CHECK LOG\n";

        // Column header row: Hour | product1 | product2 | ...
        f << "Hour";
        for (const auto& p : PRODUCT_TABLE)
            f << "," << csvEsc(p.cfaLabel);
        f << ",All PASS?\n";

        // Build lookup: hour → {productID → last reading}
        std::map<int, std::map<ProductID, const TempLogEntry*>> grid;
        for (const auto& e : logs)
            grid[e.hourSlot][e.product] = &e;

        for (int h = open; h < close; ++h) {
            f << formatHour(h);
            bool rowPass = true;
            auto& hourMap = grid[h];
            for (const auto& p : PRODUCT_TABLE) {
                auto it = hourMap.find(p.id);
                if (it == hourMap.end()) {
                    f << ",--";   // no reading taken this slot
                    rowPass = false;
                } else {
                    f << "," << std::fixed << std::setprecision(1)
                      << it->second->temperature
                      << " (" << it->second->status << ")";
                    if (!it->second->inSafeRange) rowPass = false;
                }
            }
            f << "," << (rowPass ? "YES" : "NO") << "\n";
        }
        f << "\n";
    }

    // ── Full detail rows ─────────────────────────────────────────────────
    void writeDetailSection(std::ofstream& f,
                            const std::vector<TempLogEntry>& logs)
    {
        f << "DETAILED READINGS\n";
        f << "Date,Day,Hour,Time,CFA Item,Temp (F),"
             "Min (F),Max (F),Status,Corrective Action\n";

        for (const auto& e : logs) {
            char timeBuf[16];
            std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &e.timestamp);

            f << formatTmDate(e.timestamp)          << ","
              << CFASchedule::dayName(e.weekday)    << ","
              << formatHour(e.hourSlot)             << ","
              << timeBuf                            << ","
              << csvEsc(e.cfaLabel)                << ","
              << std::fixed << std::setprecision(1)
              << e.temperature                      << ","
              << productMin(e.product)              << ","
              << productMax(e.product)              << ","
              << e.status                           << ","
              << csvEsc(e.corrective)              << "\n";
        }
        f << "\n";
    }

    // ── Totals block ─────────────────────────────────────────────────────
    void writeTotalsBlock(std::ofstream& f,
                          const std::vector<TempLogEntry>& logs)
    {
        int pass = 0, fail = 0;
        for (const auto& e : logs) {
            if (e.inSafeRange) ++pass; else ++fail;
        }
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
    }

    // ── Utilities ────────────────────────────────────────────────────────

    std::string buildFilename() const {
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        int wday = t->tm_wday;
        const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
        char buf[64];
        std::strftime(buf, sizeof(buf), "CFA_TempLog_%Y-%m-%d_", t);
        return std::string(buf) + days[wday] + ".csv";
    }

    std::string currentDateStr() const {
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        char buf[16];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", t);
        return buf;
    }

    std::string currentDateTimeStr() const {
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
        return buf;
    }

    std::string formatTmDate(const std::tm& t) const {
        char buf[16];
        std::strftime(buf, (size_t)16, "%Y-%m-%d", &t);
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

    bool fileExists(const std::string& path) const {
        std::ifstream f(path);
        return f.good();
    }

    void ensureDirectory(const std::string& path) const {
#ifdef __unix__
        std::system(("mkdir -p " + path).c_str());
#elif defined(_WIN32)
        std::system(("mkdir \"" + path + "\" 2>nul").c_str());
#endif
    }
};
