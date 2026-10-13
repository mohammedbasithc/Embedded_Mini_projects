#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

Adafruit_MPU6050 mpu;

const int greenLED = 25;
const int redLED = 26;
const float tolerance = 5.0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not detected!");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("ESP32 Leveling System Started");
}

void loop() {
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Accelerometer-based tilt angles
  float roll = atan2(
    accel.acceleration.y,
    accel.acceleration.z
  ) * 180.0 / PI;

  float pitch = atan2(
    -accel.acceleration.x,
    sqrt(
      accel.acceleration.y * accel.acceleration.y +
      accel.acceleration.z * accel.acceleration.z
    )
  ) * 180.0 / PI;

  Serial.print("Roll: ");
  Serial.print(roll, 2);
  Serial.print(" deg | Pitch: ");
  Serial.print(pitch, 2);
  Serial.print(" deg | Gyro X: ");
  Serial.print(gyro.gyro.x, 2);
  Serial.println(" rad/s");

  // Level indicator
  if (abs(roll) <= tolerance &&
      abs(pitch) <= tolerance) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    Serial.println("STATUS: LEVEL");
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    Serial.println("STATUS: TILTED");
  }

  Serial.println("-------------------------");
  delay(200);
}