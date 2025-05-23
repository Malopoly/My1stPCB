#include <Wire.h> 
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BleGamepad.h> 
//#include <syscfg/syscfg.h>

// === OBJETS ===
Adafruit_MPU6050 mpu;
BleGamepad bleGamepad; // nom visible via Bluetooth

// === DÉFINITIONS DES BROCHES ===
#define JOY_X_PIN 0       // A0
#define JOY_Y_PIN 1       // A1
#define JOY_SW_PIN 17     // Joystick bouton
#define BTN_PIN 19        // Bouton externe 1
#define BTN_PIN2 20       // Bouton externe 2
#define INVERT_X true

void setup() {
  Serial.begin(115200);
  Serial.println(F("Initialize System"));

  // Init BLE Gamepad
  bleGamepad.begin();

  // Init MPU6050
  if (!mpu.begin(0x68)) {
    Serial.println("MPU6050 not found");
    while (1) delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Init entrées
  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN2, INPUT_PULLUP);
}

void loop() {
  // Lecture MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.println("=== MPU6050 ===");
  Serial.printf("Acc X: %.2fg, Y: %.2fg, Z: %.2fg\n", a.acceleration.x / 9.81, a.acceleration.y / 9.81, a.acceleration.z / 9.81);
  Serial.printf("Gyro X: %.1f°/s, Y: %.1f°/s, Z: %.1f°/s\n", g.gyro.x * 180 / PI, g.gyro.y * 180 / PI, g.gyro.z * 180 / PI);
  Serial.printf("Temp: %.1f°C\n", temp.temperature);

  // === JOYSTICK ===
  int joyX = analogRead(JOY_X_PIN);
  int joyY = analogRead(JOY_Y_PIN);
  bool joyPressed = (digitalRead(JOY_SW_PIN) == LOW);

  // Normalisation en valeurs de joystick (-32767 à 32767)
  int joyX_mapped = map(joyX, 4095, 0, 0, 32737);
  int joyY_mapped = map(joyY, 4095, 0, 0, 32737);

  if (abs(joyX_mapped) < 20) joyX_mapped = 0;
  if (abs(joyY_mapped) < 20) joyY_mapped = 0;

if (INVERT_X) {
      joyX_mapped = 32737 - joyX_mapped;
      if (joyX_mapped < 0)
        joyX_mapped = -joyX_mapped;
    }


  Serial.println("=== Joystick ===");
  Serial.printf("X: %d, Y: %d, Pressed: %s\n", joyX_mapped, joyY_mapped, joyPressed ? "YES" : "NO");

  // === BOUTONS ===
  bool btn1 = (digitalRead(BTN_PIN) == LOW);
  bool btn2 = (digitalRead(BTN_PIN2) == LOW);
  Serial.printf("Button 1: %s | Button 2: %s\n", btn1 ? "YES" : "NO", btn2 ? "YES" : "NO");

  // === BLE GAMEPAD ===
  if (bleGamepad.isConnected()) {
    // Joystick gauche (left thumbstick)
    bleGamepad.setLeftThumb(joyX_mapped, joyY_mapped);

    // Boutons
    if (joyPressed) bleGamepad.press(1); else bleGamepad.release(1);
    if (btn1)       bleGamepad.press(2); else bleGamepad.release(2);
    if (btn2)       bleGamepad.press(3); else bleGamepad.release(3);

    bleGamepad.sendReport();
  }

  delay(50); // fréquence d’envoi BLE
}