#include "esphome.h"
#include "ad9833_component.h"
#include "mcp41010_component.h"

using namespace esphome;

AD9833Component *ad9833;
MCP41010Component *mcp41010;

class SignalGenApp : public Component {
 public:
  void setup() override {
    ad9833 = new AD9833Component(5);   // CS pin GPIO5
    ad9833->setup();

    mcp41010 = new MCP41010Component(4); // CS pin GPIO4
    mcp41010->setup();

    ESP_LOGI("signalgen", "Signal Generator setup complete");
  }

  void loop() override {
    // Sweep logic here
    if (ad9833->isSweepEnabled()) {
      static uint32_t last_update = 0;
      uint32_t now = millis();
      if (now - last_update >= ad9833->getSweepInterval()) {
        last_update = now;
        uint32_t freq = ad9833->getCurrentFreq();

        if (strcmp(ad9833->getSweepModeName(), "Lin") == 0) {
          freq += ad9833->getSweepStep();
          if (freq > ad9833->getSweepEnd()) freq = ad9833->getSweepStart();
        } else if (strcmp(ad9833->getSweepModeName(), "Log") == 0) {
          freq = (uint32_t)(freq * ad9833->getSweepFactor());
          if (freq > ad9833->getSweepEnd()) freq = ad9833->getSweepStart();
        }

        ad9833->setFrequency(0, freq);
      }
    }
  }
};