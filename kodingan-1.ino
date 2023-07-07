#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

// Konfigurasi pin
const int PIR_PIN = 2;   // Pin untuk sensor PIR
const int IN1_PIN = 9;   // Pin IN1 LN298N
const int IN2_PIN = 8;   // Pin IN2 LN298N
const int ENA_PIN = 10;  // Pin ENA LN298N

// Konfigurasi motor DVD ROM
const int MOTOR_SPEED = 120;  // Kecepatan motor (semakin kecil semakin lambat)

// Konfigurasi LCD 1602
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Variabel status pintu
enum DoorStatus { CLOSED,
                  OPEN };
DoorStatus doorStatus = CLOSED;

// Variabel deteksi gerakan
int motionDetected = 0;
int motionPinState = LOW;

// Waktu penundaan sebelum mematikan motor
const unsigned long DOOR_CLOSE_DELAY = 2000;  // 5 detik
unsigned long doorCloseTime = 0;

const char* openingText = "Selamat Datang! ";
const char* openingText1 = "Pasti Dadi ";
void setup() {
  // Inisialisasi pin dan serial monitor
  pinMode(PIR_PIN, INPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);

  digitalWrite(ENA_PIN, HIGH);  // Aktifkan motor DVD ROM

  Serial.begin(9600);

  lcd.begin(16, 2); 
  lcd.setCursor(0, 0);
  for (int i = 0; i < 16; i++) {
    lcd.print(openingText[i]);
    delay(200);
  }
  lcd.setCursor(6,2);
  for (int i = 0; i < 16; i++) {
    lcd.print(openingText1[i]);
    delay(200);
  }
}

void loop() {
  // Deteksi gerakan menggunakan sensor PIR
  motionDetected = digitalRead(PIR_PIN);

  if (motionDetected != motionPinState) {
    if (motionDetected == HIGH) {
      // Gerakan terdeteksi, buka pintu
      openDoor();
    } else {
      // Gerakan berhenti, cek jeda dan tutup pintu
      if (doorStatus == OPEN) {
        doorCloseTime = millis() + DOOR_CLOSE_DELAY;
      }
    }

    motionPinState = motionDetected;
  }

  // Periksa jeda sebelum mematikan motor
  if (doorCloseTime > 0 && millis() >= doorCloseTime) {
    closeDoor();
    doorCloseTime = 0;  // Reset waktu penutupan pintu
  }
}

void openDoor() {
  if (doorStatus == CLOSED) {
    // Buka pintu menggunakan motor DVD ROM
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    analogWrite(ENA_PIN, MOTOR_SPEED);

    doorStatus = OPEN;
    lcd.clear();
    lcd.setCursor(0, 0);  // Inisialisasi LCD
    lcd.print("Pintu: terbuka!");
    Serial.println("Pintu terbuka!");
  }
}

void closeDoor() {
  if (doorStatus == OPEN) {
    // Tutup pintu menggunakan motor DVD ROM
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    analogWrite(ENA_PIN, MOTOR_SPEED);

    delay(1000);  // Tambahkan penundaan untuk menutup pintu

    digitalWrite(ENA_PIN, LOW);  // Matikan motor DVD ROM

    doorStatus = CLOSED;
    lcd.clear();
    lcd.setCursor(0, 2);
    lcd.print("Pintu: tertutup!");
    Serial.println("Pintu tertutup!");
  }
}
