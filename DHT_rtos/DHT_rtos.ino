#include <Arduino.h>
#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LED_PIN 2
#define TEMP_THRESHOLD 30.0

DHT dht(DHT_PIN, DHT_TYPE);

// Data structure for temperature and humidity
struct SensorData {
  float temperature;
  float humidity;
};

// Two queues
QueueHandle_t queue1;
QueueHandle_t queue2;

// Task 1: Read DHT11 every 2 seconds
void SenseTask(void *parameter) {
  SensorData data;

  while (1) {
    data.temperature = dht.readTemperature();
    data.humidity = dht.readHumidity();

    if (!isnan(data.temperature) &&
        !isnan(data.humidity)) {

      xQueueOverwrite(queue1, &data);
      xQueueOverwrite(queue2, &data);
    } else {
      Serial.println("DHT11 read failed!");
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

// Task 2: Print temperature and humidity
void PublishTask(void *parameter) {
  SensorData data;

  while (1) {
    if (xQueueReceive(queue1, &data, portMAX_DELAY)) {
      Serial.print("Temperature: ");
      Serial.print(data.temperature);
      Serial.print(" C, Humidity: ");
      Serial.print(data.humidity);
      Serial.println(" %");
    }
  }
}

// Task 3: Check temperature and control LED
void AlertTask(void *parameter) {
  SensorData data;

  while (1) {
    if (xQueueReceive(queue2, &data, portMAX_DELAY)) {
      if (data.temperature >= TEMP_THRESHOLD) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();

  queue1 = xQueueCreate(1, sizeof(SensorData));
  queue2 = xQueueCreate(1, sizeof(SensorData));

  if (queue1 == NULL || queue2 == NULL) {
    Serial.println("Queue creation failed!");
    while (1) {
      delay(1000);
    }
  }

  xTaskCreate(SenseTask, "Sense", 3072, NULL, 2, NULL);
  xTaskCreate(PublishTask, "Publish", 3072, NULL, 1, NULL);
  xTaskCreate(AlertTask, "Alert", 3072, NULL, 2, NULL);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}