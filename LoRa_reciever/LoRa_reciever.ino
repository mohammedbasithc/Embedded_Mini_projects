#include <SPI.h>
#include <LoRa.h>

// SPI pins for ESP32
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23
#define SS_PIN     5
#define RST_PIN   14
#define DIO0_PIN  26

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("LoRa Receiver Starting...");

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed!");
    Serial.println("Check wiring and 433 MHz module.");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("LoRa Receiver Ready!");
  Serial.println("Waiting for messages...");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize > 0) {
    String receivedMessage = "";

    while (LoRa.available()) {
      receivedMessage += (char)LoRa.read();
    }

    Serial.print("Received: ");
    Serial.println(receivedMessage);

    Serial.print("RSSI: ");
    Serial.print(LoRa.packetRssi());
    Serial.println(" dBm");

    Serial.print("SNR: ");
    Serial.print(LoRa.packetSnr());
    Serial.println(" dB");

    Serial.println("----------------------");
  }
}