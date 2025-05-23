#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// MPU6050
Adafruit_MPU6050 mpu;

// Joystick pins
#define JOY_X_PIN 0
#define JOY_Y_PIN 1
#define JOY_SW_PIN 17

// External button
#define BTN_PIN 19
#define BTN_PIN2 20

void setup() {
  Serial.begin(115200);
  Serial.println(F("Initialize System"));

  if (!mpu.begin(0x68)) {
    Serial.println("MPU6050 not found");
    while (1) delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN2, INPUT_PULLUP);
}

void loop() {
  // MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.println("=== MPU6050 ===");
  Serial.printf("Acc X: %.2fg, Y: %.2fg, Z: %.2fg\n", a.acceleration.x / 9.81, a.acceleration.y / 9.81, a.acceleration.z / 9.81);
  Serial.printf("Gyro X: %.1f°/s, Y: %.1f°/s, Z: %.1f°/s\n", g.gyro.x * 180 / PI, g.gyro.y * 180 / PI, g.gyro.z * 180 / PI);
  Serial.printf("Temp: %.1f°C\n", temp.temperature);

  // Joystick
  int joyX = analogRead(JOY_X_PIN);
  int joyY = analogRead(JOY_Y_PIN);
  bool joyPressed = digitalRead(JOY_SW_PIN) == LOW;

  float joyX_norm = (joyX - 2048) / 2048.0;
  float joyY_norm = (joyY - 2048) / 2048.0;

  Serial.println("=== Joystick ===");
  Serial.printf("X: %.2f, Y: %.2f, Pressed: %s\n", joyX_norm, joyY_norm, joyPressed ? "YES" : "NO");

  // Button
  bool btnPressed = digitalRead(BTN_PIN) == LOW;
  Serial.printf("Button Pressed: %s\n", btnPressed ? "YES" : "NO");
  bool btnPressed2 = digitalRead(BTN_PIN2) == LOW;
  Serial.printf("Button Pressed2: %s\n", btnPressed2 ? "YES" : "NO");

  delay(200);
}