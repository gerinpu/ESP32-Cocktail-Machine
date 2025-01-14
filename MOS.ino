#include "Adafruit_INA3221.h"
#include <Wire.h>

Adafruit_INA3221 ina3221;
bool dataEnabled = false;
int interval = 20;
int duration = 0;
unsigned long startTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  if (!ina3221.begin(0x40, &Wire)) {
    Serial.println("Failed to find INA3221 chip");
    while (1)
      delay(10);
  }
  Serial.println("INA3221 Found!");

  ina3221.setAveragingMode(INA3221_AVG_16_SAMPLES);

  for (uint8_t i = 0; i < 3; i++) {
    ina3221.setShuntResistance(i, 0.05);
  }

  ina3221.setPowerValidLimits(3.0 /* lower limit */, 15.0 /* upper limit */);
}

void collectData() {
  if (dataEnabled) {
    unsigned long currentTime = millis();
    if (currentTime - startTime >= duration * 1000) {
      Serial.println("Done.");
      dataEnabled = false;
      return;
    }

    static unsigned long lastTransmitTime = 0;
    if (currentTime - lastTransmitTime >= interval) {
      float voltage = ina3221.getBusVoltage(0);
      float current = ina3221.getCurrentAmps(0) * 1000;

      Serial.print("Voltage = ");
      Serial.print(voltage, 2);
      Serial.print(" V, Current = ");
      Serial.print(current, 2);
      Serial.println(" mA");

      lastTransmitTime = currentTime;
    }
  }
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 's') {
      dataEnabled = false;
      Serial.println("Data collection stopped.");
    } else if (command == 'r') {
      Serial.println("Enter interval (ms) and duration (s):");

      // Read interval
      while (Serial.available() == 0) {}
      interval = Serial.parseInt();
      if (interval <= 0) {
        Serial.println("Invalid interval. Please enter a positive value.");
        return;
      }
      Serial.print("Interval: ");
      Serial.println(interval);

      // Read duration
      while (Serial.available() == 0) {}
      duration = Serial.parseInt();
      if (duration <= 0) {
        Serial.println("Invalid duration. Please enter a positive value.");
        return;
      }
      Serial.print("Duration: ");
      Serial.println(duration);

      // Start data collection
      dataEnabled = true;
      startTime = millis();
    }
  }else{
// Collect data
  collectData();
  }

  delay(interval);
  
}
