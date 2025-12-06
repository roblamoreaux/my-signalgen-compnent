#pragma once
#include "esphome.h"
//#include <SPI.h>

class AD9833Component {
 public:
  enum Waveform {
    WAVEFORM_SINE,
    WAVEFORM_TRIANGLE,
    WAVEFORM_SQUARE
  };

  enum SweepMode {
    SWEEP_OFF,
    SWEEP_LINEAR,
    SWEEP_LOG
  };

  AD9833Component(int cs_pin) : cs_pin_(cs_pin) {}

  void setup() {
    pinMode(cs_pin_, OUTPUT);
    digitalWrite(cs_pin_, HIGH);
    SPI.begin();
    setWaveform(WAVEFORM_SINE);
    setFrequency(0, 1000);
  }

  void setFrequency(uint8_t reg, uint32_t freq) {
    current_freq_ = freq;
    // SPI write sequence to AD9833
    uint32_t freq_word = (uint32_t)((float)freq * 268435456.0 / 25000000.0);
    uint16_t lsb = (uint16_t)(freq_word & 0x3FFF) | 0x4000;
    uint16_t msb = (uint16_t)((freq_word >> 14) & 0x3FFF) | 0x4000;
    writeRegister(0x2100); // control reset
    writeRegister(lsb);
    writeRegister(msb);
    writeRegister(0x2000); // clear reset
  }

  void setWaveform(Waveform wf) {
    current_waveform_ = wf;
    switch (wf) {
      case WAVEFORM_SINE: writeRegister(0x2000); break;
      case WAVEFORM_TRIANGLE: writeRegister(0x2002); break;
      case WAVEFORM_SQUARE: writeRegister(0x2028); break;
    }
  }

  void nextWaveform() {
    if (current_waveform_ == WAVEFORM_SINE) setWaveform(WAVEFORM_TRIANGLE);
    else if (current_waveform_ == WAVEFORM_TRIANGLE) setWaveform(WAVEFORM_SQUARE);
    else setWaveform(WAVEFORM_SINE);
  }

  const char* getWaveformName() {
    switch (current_waveform_) {
      case WAVEFORM_SINE: return "Sine";
      case WAVEFORM_TRIANGLE: return "Tri";
      case WAVEFORM_SQUARE: return "Sq";
      default: return "?";
    }
  }

  void enableSweep(bool en) { sweep_enabled_ = en; }
  bool isSweepEnabled() { return sweep_enabled_; }

  void setSweepMode(SweepMode mode) { sweep_mode_ = mode; }
  const char* getSweepModeName() {
    switch (sweep_mode_) {
      case SWEEP_LINEAR: return "Lin";
      case SWEEP_LOG: return "Log";
      default: return "Off";
    }
  }

  void setSweepStart(uint32_t f) { sweep_start_ = f; }
  void setSweepEnd(uint32_t f) { sweep_end_ = f; }
  void setSweepStep(uint32_t s) { sweep_step_ = s; }
  void setSweepInterval(uint32_t i) { sweep_interval_ = i; }
  void setSweepFactor(float f) { sweep_factor_ = f; }

  uint32_t getSweepStart() { return sweep_start_; }
  uint32_t getSweepEnd() { return sweep_end_; }
  uint32_t getSweepStep() { return sweep_step_; }
  uint32_t getSweepInterval() { return sweep_interval_; }
  uint32_t getCurrentFreq() { return current_freq_; }

  void startFreqCycle(uint32_t interval_ms) {
    // Example: alternate between FREQ0 and FREQ1
    setFrequency(0, sweep_start_);
    delay(interval_ms);
    setFrequency(0, sweep_end_);
  }

  void setGainCompFactor(float f) { gain_comp_factor_ = f; }
  void enableGainTracking(bool en) { gain_tracking_ = en; }

 private:
  int cs_pin_;
  Waveform current_waveform_ = WAVEFORM_SINE;
  SweepMode sweep_mode_ = SWEEP_OFF;
  uint32_t current_freq_ = 1000;
  bool sweep_enabled_ = false;
  bool gain_tracking_ = false;
  float gain_comp_factor_ = 1.0;

  uint32_t sweep_start_ = 20;
  uint32_t sweep_end_ = 20000;
  uint32_t sweep_step_ = 100;
  uint32_t sweep_interval_ = 1000;
  float sweep_factor_ = 1.122;

  void writeRegister(uint16_t data) {
    digitalWrite(cs_pin_, LOW);
    SPI.transfer16(data);
    digitalWrite(cs_pin_, HIGH);
  }
};