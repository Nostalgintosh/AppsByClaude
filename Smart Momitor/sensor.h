#pragma once
#include <cmath>
#include <cstdlib>

// ═══════════════════════════════════════════════════════════════════════════
//  sensor.h  —  Temperature sensor abstraction layer
//  Swap SimulatedSensor for DS18B20Sensor when deploying on hardware.
// ═══════════════════════════════════════════════════════════════════════════

class TemperatureSensor {
public:
    virtual ~TemperatureSensor() = default;
    virtual bool  init()              = 0;
    virtual void  startMeasurement()  = 0;
    virtual bool  isReady()     const = 0;
    virtual float readFahrenheit()    = 0;
    virtual bool  selfTest()          { return true; }

    float readCelsius() {
        float f = readFahrenheit();
        if (std::isnan(f)) return f;
        return (f - 32.0f) * 5.0f / 9.0f;
    }
};

// ─────────────────────────────────────────────
//  SIMULATED SENSOR  (desktop / unit-test use)
// ─────────────────────────────────────────────
class SimulatedSensor : public TemperatureSensor {
public:
    explicit SimulatedSensor(float base = 160.0f, float noise = 1.5f)
        : base_(base), noise_(noise) {}

    bool init() override { return true; }

    void startMeasurement() override {
        float r = ((float)std::rand() / RAND_MAX) * 2.0f - 1.0f;
        last_ = base_ + r * noise_;
        ready_ = true;
    }

    bool  isReady()       const override { return ready_; }
    float readFahrenheit()      override { ready_ = false; return last_; }

    void  setBaseTemp(float t)           { base_ = t; }

private:
    float base_, noise_, last_ = 0;
    bool  ready_ = false;
};

// ─────────────────────────────────────────────
//  DS18B20  (real hardware – fill in GPIO calls)
// ─────────────────────────────────────────────
class DS18B20Sensor : public TemperatureSensor {
public:
    explicit DS18B20Sensor(int gpio) : gpio_(gpio) {}

    bool init() override {
        // TODO: oneWire.begin(gpio_); sensor.setResolution(12);
        return true;
    }
    void startMeasurement() override {
        // TODO: sensor.requestTemperatures();
        req_ = true;
    }
    bool isReady() const override {
        // TODO: return sensor.isConversionComplete();
        return req_;
    }
    float readFahrenheit() override {
        req_ = false;
        // TODO: return sensor.getTempFByIndex(0);
        return 160.0f; // placeholder until wired
    }

private:
    int  gpio_;
    bool req_ = false;
};
