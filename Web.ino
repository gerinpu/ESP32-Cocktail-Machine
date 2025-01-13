#include "Adafruit_INA3221.h"
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>

Adafruit_INA3221 ina3221;

// Replace with your network credentials
const char* ssid = "HappyNet";
const char* password = "a938dfef7a5d53a5";

WebServer server(80);

String dataBuffer = "";
boolean collect = false;
int duration = 30;
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

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  // Set up the web server
  server.on("/", []() {
    server.send(200, "text/html", "<html><head><meta http-equiv=\"refresh\" content=\"5\"></head><body><pre>" + dataBuffer + "</pre></body></html>");
  });

  server.on("/start", []() {
      collect = true;
      startTime = millis(); 
      duration = 30; // Set duration to 2 minutes (120 seconds)

    dataBuffer = "";

    server.send(200, "text/html", "Data collection started for 2 minutes...");
  });

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (collect) {
    float current = ina3221.getCurrentAmps(0) * 1000;

    String newData = /*String(millis() / 1000.0, 2) + "s: " +  */String(current, 2) ;
    int len = newData.length();
    dataBuffer += newData;

   
    if (dataBuffer.length() > 65000) {
     dataBuffer = dataBuffer.substring(dataBuffer.length() - 65000);
    }

    Serial.print(len);

    if (millis() - startTime >= duration * 1000) {
      collect = false;
      Serial.println("Data collection stopped.");
    }
  
  }
  delay(10);
}