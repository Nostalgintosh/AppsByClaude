#pragma once

// ═══════════════════════════════════════════════════════════════════════════
//  CYPHAMOMETER — CFA Food Safety Thermometer
//  thermometer.h  —  Core types, products, CFA day-aware schedule
// ═══════════════════════════════════════════════════════════════════════════

#include <string>
#include <vector>
#include <ctime>
#include <functional>

// ─────────────────────────────────────────────
//  PRODUCT IDs  (one per physical button 1-8)
// ─────────────────────────────────────────────
enum class ProductID {
    NONE       = 0,
    REGULAR,        // BTN 1 – Original Chicken Sandwich
    SPICY,          // BTN 2 – Spicy Deluxe
    GRILLED,        // BTN 3 – Grilled Chicken Filet/Sandwich
    CJ,             // BTN 4 – Nuggets / Chick-n-Strips
    PJ_AMERO,       // BTN 5 – Mac & Cheese / Soup (hot side item)
    LETTUCE,        // BTN 6 – Lettuce / Cold Toppings
    TOMATOES,       // BTN 7 – Tomatoes / Sliced Produce
    FRIES           // BTN 8 – Waffle Fries
};

// ─────────────────────────────────────────────
//  PRODUCT TABLE
//  Temps: Chick-fil-A SAFE program + FDA Food Code
//    Hot-hold  ≥ 135 °F  (CFA internal standard: 140 °F min)
//    Cold-hold ≤  41 °F  (CFA cold target: 34–41 °F)
// ─────────────────────────────────────────────
struct Product {
    ProductID   id;
    std::string name;           // short display name (LCD row)
    std::string cfaLabel;       // official CFA temp-log label
    float       minSafeTemp;    // °F inclusive lower bound
    float       maxSafeTemp;    // °F inclusive upper bound
    bool        isHot;          // true = hot-hold  false = cold-hold
};

inline const std::vector<Product> PRODUCT_TABLE = {
    //  ID                    Display Name       CFA Log Label            Min    Max    Hot
    { ProductID::REGULAR,  "Original CFS",    "Chicken Sandwich",       140.0f, 180.0f, true  },
    { ProductID::SPICY,    "Spicy Deluxe",    "Spicy Deluxe",           140.0f, 180.0f, true  },
    { ProductID::GRILLED,  "Grilled Filet",   "Grilled Sandwich",       140.0f, 180.0f, true  },
    { ProductID::CJ,       "Nuggets/Strips",  "Nuggets & Strips",       140.0f, 180.0f, true  },
    { ProductID::PJ_AMERO, "Mac & Cheese",    "Mac & Cheese / Soup",    140.0f, 180.0f, true  },
    { ProductID::LETTUCE,  "Lettuce",         "Cold Toppings",           34.0f,  41.0f, false },
    { ProductID::TOMATOES, "Tomatoes",        "Tomatoes / Produce",      34.0f,  41.0f, false },
    { ProductID::FRIES,    "Waffle Fries",    "Waffle Fries",           135.0f, 180.0f, true  },
};

// ─────────────────────────────────────────────
//  TEMPERATURE LOG ENTRY
// ─────────────────────────────────────────────
struct TempLogEntry {
    std::tm     timestamp;      // local wall-clock time
    int         weekday;        // tm_wday 0=Sun 1=Mon … 6=Sat
    int         hourSlot;       // 11-21 (M-Th) or 11-23 (F/Sat)
    ProductID   product;
    std::string productName;
    std::string cfaLabel;
    float       temperature;    // °F
    bool        inSafeRange;
    std::string status;         // "PASS" | "FAIL-HOT" | "FAIL-COLD"
    std::string corrective;     // free-text corrective action (blank if PASS)
};

// ─────────────────────────────────────────────
//  CHICK-FIL-A OPERATING SCHEDULE
//
//  Mon–Thu  11:00 AM – 9:59 PM   hourly checks: 11,12,13,...,21
//  Fri–Sat  11:00 AM – 11:59 PM  hourly checks: 11,12,13,...,23
//  Sunday   CLOSED
// ─────────────────────────────────────────────
struct CFASchedule {
    static constexpr int OPEN_HOUR      = 11;
    static constexpr int CLOSE_MTH      = 22;   // Mon-Thu: last check slot=21, close=22
    static constexpr int CLOSE_FS       = 24;   // Fri-Sat: last check slot=23, close=24

    // Returns 0 on Sunday (closed), else exclusive close hour
    static int closeHour(int wday) {
        if (wday == 0)              return 0;   // Sunday – closed all day
        if (wday >= 1 && wday <= 4) return CLOSE_MTH;
        return CLOSE_FS;                        // wday 5=Fri, 6=Sat
    }

    static bool isOpenDay(int wday)              { return wday != 0; }
    static bool isOperating(int wday, int hour)  {
        return isOpenDay(wday) && hour >= OPEN_HOUR && hour < closeHour(wday);
    }

    // Human-readable hours string for LCD / CSV
    static std::string hoursStr(int wday) {
        if (!isOpenDay(wday))       return "CLOSED (Sunday)";
        if (closeHour(wday) == CLOSE_MTH) return "11:00 AM - 9:59 PM";
        return "11:00 AM - 11:59 PM";
    }

    static std::string dayName(int wday) {
        static const char* N[] = {"Sunday","Monday","Tuesday","Wednesday",
                                  "Thursday","Friday","Saturday"};
        return (wday >= 0 && wday <= 6) ? N[wday] : "?";
    }
};

// ─────────────────────────────────────────────
//  STATE MACHINE
// ─────────────────────────────────────────────
enum class ThermometerState {
    IDLE,
    PRODUCT_SELECTED,
    MEASURING,
    RESULT_DISPLAY,
    HOUR_RESET,
    USB_EXPORT,
    CLOSED
};

// ─────────────────────────────────────────────
//  BUTTON IDs
// ─────────────────────────────────────────────
enum class ButtonID {
    BTN_REGULAR   = 0,
    BTN_SPICY     = 1,
    BTN_GRILLED   = 2,
    BTN_CJ        = 3,
    BTN_PJ_AMERO  = 4,
    BTN_LETTUCE   = 5,
    BTN_TOMATOES  = 6,
    BTN_FRIES     = 7,
    BTN_START     = 8,
    BTN_END       = 9,
    BTN_NONE      = -1
};
