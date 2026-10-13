#include "AdafruitIO_WiFi.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

// Wi-Fi credentials
#define WIFI_SSID       "YOUR_WIFI_NAME"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

// Adafruit IO credentials
#define IO_USERNAME     "YOUR_ADAFRUIT_USERNAME"
#define IO_KEY          "YOUR_ADAFRUIT_IO_KEY"

// Connect to Adafruit IO
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY,
                   WIFI_SSID, WIFI_PASSWORD);

// Adafruit IO feeds
AdafruitIO_Feed *temperatureFeed = io.feed("temperature");
AdafruitIO_Feed *pressureFeed = io.feed("pressure");
AdafruitIO_Feed *altitudeFeed = io.feed("altitude");

// BMP180 sensor
Adafruit_BMP085 bmp;

unsigned long lastUpdate = 0;
const unsigned long interval = 10000; // 10 seconds

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  // Initialize BMP180
  if (!bmp.begin()) {
    Serial.println("BMP180 not detected!");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("BMP180 initialized");

  // Connect to Adafruit IO
  Serial.println("Connecting to Adafruit IO...");
  io.connect();

  while (io.status() < AIO_CONNECTED) {
    io.run();
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Connected to Adafruit IO!");
}

void loop() {
  // Keep the cloud connection active
  io.run();

  if (millis() - lastUpdate >= interval) {
    lastUpdate = millis();

    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0F;
    float altitude = bmp.readAltitude();

    Serial.println("\n--- BMP180 Readings ---");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Altitude: ");
    Serial.print(altitude);
    Serial.println(" m");

    // Upload values to Adafruit IO
    temperatureFeed->save(temperature);
    pressureFeed->save(pressure);
    altitudeFeed->save(altitude);

    Serial.println("Readings submitted to Adafruit IO");
  }
}