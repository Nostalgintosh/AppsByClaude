#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cstdio>

// ═══════════════════════════════════════════════════════════════════════════
//  display.h  —  Display abstraction layer
//  ConsoleDisplay  : for desktop testing / simulation
//  LCD20x4Display  : stub for real 20×4 I2C LCD hardware
// ═══════════════════════════════════════════════════════════════════════════

class Display {
public:
    virtual ~Display() = default;
    virtual void clear() = 0;
    virtual void print(int row, int col, const std::string& text) = 0;
    virtual void setBacklight(bool on) = 0;
    virtual void showAlert(bool isError) = 0;   // red/green LED + buzzer
    virtual void refresh() {}
};

// ─────────────────────────────────────────────
//  CONSOLE DISPLAY  (terminal simulation)
// ─────────────────────────────────────────────
class ConsoleDisplay : public Display {
    static constexpr int COLS = 20;
    static constexpr int ROWS = 4;
public:
    ConsoleDisplay() { rows_.assign(ROWS, std::string(COLS, ' ')); }

    void clear() override {
        for (auto& r : rows_) r = std::string(COLS, ' ');
        redraw();
    }

    void print(int row, int col, const std::string& text) override {
        if (row < 0 || row >= ROWS) return;
        auto& r = rows_[row];
        for (int i = 0; i < (int)text.size(); ++i) {
            int p = col + i;
            if (p >= 0 && p < COLS) r[p] = text[i];
        }
        redraw();
    }

    void setBacklight(bool on) override {
        std::cout << (on ? "[LCD ON]\n" : "[LCD OFF]\n");
    }

    void showAlert(bool isError) override {
        if (isError)
            std::cout << "*** !! TEMPERATURE OUT OF SAFE RANGE !! ***\n";
        else
            std::cout << "*** OK  TEMPERATURE IN SAFE RANGE ***\n";
    }

private:
    std::vector<std::string> rows_;

    void redraw() const {
        std::cout << "\033[2J\033[H";   // clear terminal
        std::cout << " ╔════════════════════╗\n";
        for (const auto& r : rows_)
            std::cout << " ║" << std::left << std::setw(COLS) << r << "║\n";
        std::cout << " ╚════════════════════╝\n";
    }
};

// ─────────────────────────────────────────────
//  LCD 20×4 HARDWARE STUB
//  Fill in your MCU's LiquidCrystal_I2C calls
// ─────────────────────────────────────────────
class LCD20x4Display : public Display {
public:
    explicit LCD20x4Display(int i2cAddr = 0x27) : addr_(i2cAddr) {}

    bool init() {
        // lcd.begin(20, 4);
        // lcd.setBacklight(255);
        return true;
    }

    void clear()                               override { /* lcd.clear(); */          }
    void print(int row, int col,
               const std::string& text)        override { /* lcd.setCursor(col,row);
                                                             lcd.print(text.c_str()); */ }
    void setBacklight(bool on)                 override { /* lcd.setBacklight(on?255:0); */ }
    void showAlert(bool isError)               override {
        // digitalWrite(isError ? PIN_LED_RED : PIN_LED_GREEN, HIGH);
        // tone(PIN_BUZZER, isError ? 880 : 1760, 400);
    }

private:
    int addr_;
};
