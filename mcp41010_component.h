#pragma once
#include "esphome.h"
//#include <SPI.h>

class MCP41010Component {
 public:
  MCP41010Component(int cs_pin) : cs_pin_(cs_pin) {}

  void setup() {
    pinMode(cs_pin_, OUTPUT);
    digitalWrite(cs_pin_, HIGH);
    SPI.begin();
    setGain(128); // mid-scale default
  }

  void setGain(uint8_t value) {
    gain_ = value;
    digitalWrite(cs_pin_, LOW);
    SPI.transfer(0x11);   // command byte: write to pot0
    SPI.transfer(gain_);
    digitalWrite(cs_pin_, HIGH);
  }

  uint8_t getGain() { return gain_; }

 private:
  int cs_pin_;
  uint8_t gain_ = 128;
};