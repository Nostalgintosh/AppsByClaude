#pragma once

#include <ctime>
#include <functional>
#include <string>
#include "thermometer.h"

// ═══════════════════════════════════════════════════════════════════════════
//  timer.h  —  CFA day-aware hourly timer
//
//  Fires onHourReset callback at the top of every operating hour.
//  Knows about Mon-Thu vs Fri-Sat closing times, and Sunday closure.
//  On embedded hardware swap tick() polling for an RTC interrupt.
// ═══════════════════════════════════════════════════════════════════════════

class CFAHourlyTimer {
public:
    using Callback = std::function<void(int wday, int hour)>;

    CFAHourlyTimer() : lastHour_(-1), lastWday_(-1) {}

    // Register callback fired at the start of each new operating hour
    void onHourReset(Callback cb) { callback_ = cb; }

    // Call from main loop (polling); on hardware use RTC alarm interrupt
    void tick() {
        std::time_t now = std::time(nullptr);
        std::tm* t = std::localtime(&now);
        int wday = t->tm_wday;
        int hour = t->tm_hour;

        bool hourChanged = (hour != lastHour_ || wday != lastWday_);
        if (hourChanged) {
            lastHour_ = hour;
            lastWday_ = wday;
            if (CFASchedule::isOperating(wday, hour) && callback_) {
                callback_(wday, hour);
            }
        }
    }

    // ── Status queries ─────────────────────────

    bool isOpen() const {
        auto t = nowTm();
        return CFASchedule::isOperating(t.tm_wday, t.tm_hour);
    }

    bool isSunday() const {
        return nowTm().tm_wday == 0;
    }

    int currentHour() const  { return nowTm().tm_hour; }
    int currentWday() const  { return nowTm().tm_wday; }

    // Seconds remaining in current hour
    int secondsToNextHour() const {
        auto t = nowTm();
        return 60 * (59 - t.tm_min) + (60 - t.tm_sec);
    }

    // Which check slots are expected today (e.g. 11..21 or 11..23)
    int lastCheckSlot() const {
        int close = CFASchedule::closeHour(currentWday());
        return close > 0 ? close - 1 : -1;
    }

private:
    int      lastHour_, lastWday_;
    Callback callback_;

    static std::tm nowTm() {
        std::time_t n = std::time(nullptr);
        return *std::localtime(&n);
    }
};

// ─────────────────────────────────────────────
//  DATE / TIME HELPERS
// ─────────────────────────────────────────────

inline std::string formatTime(const std::tm& t) {
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
    return buf;
}

// "11:00 AM", "09:00 PM", "11:00 PM"
inline std::string formatHour(int h) {
    char buf[16];
    const char* ap = (h < 12 || h == 24) ? "AM" : "PM";
    int h12 = h % 12;
    if (h12 == 0) h12 = 12;
    std::snprintf(buf, sizeof(buf), "%02d:00 %s", h12, ap);
    return buf;
}

// "11 AM", "9 PM"
inline std::string formatHourShort(int h) {
    char buf[10];
    const char* ap = (h < 12) ? "AM" : "PM";
    int h12 = h % 12;
    if (h12 == 0) h12 = 12;
    std::snprintf(buf, sizeof(buf), "%d%s", h12, ap);
    return buf;
}

inline std::tm getCurrentTm() {
    std::time_t now = std::time(nullptr);
    return *std::localtime(&now);
}
