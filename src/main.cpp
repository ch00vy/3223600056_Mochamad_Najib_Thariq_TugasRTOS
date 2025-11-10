#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>

// -------------------- Pin Mapping (sudah diperbaiki) --------------------
#define PIN_LED1   4
#define PIN_LED2   5
#define PIN_LED3   6

#define PIN_BTN1   9   // tombol ke GND (INPUT_PULLUP)
#define PIN_BTN2   10

#define PIN_BUZZER 3    // PWM (LEDC)

#define PIN_OLED_SDA 17
#define PIN_OLED_SCL 18

#define PIN_POT    2    // ADC1_0

#define PIN_ENC_A  13
#define PIN_ENC_B  14
#define PIN_ENC_SW 12    // tombol encoder (opsional, ke GND)

// PIN STEPPER DIPINDAHKAN DARI 38,39,40 (BERBAHAYA!)
#define PIN_STP_IN1 47
#define PIN_STP_IN2 39  // <-- Diubah dari 39
#define PIN_STP_IN3 40  // <-- Diubah dari 40
#define PIN_STP_IN4 38  // <-- Diubah dari 38

#define PIN_SERVO  11

// -------------------- Peripherals --------------------
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Stepper 28BYJ-48 + ULN2003 (urutan HALF4WIRE: IN1, IN3, IN2, IN4)
AccelStepper stepper(AccelStepper::HALF4WIRE, PIN_STP_IN1, PIN_STP_IN3, PIN_STP_IN2, PIN_STP_IN4);

Servo servo1;

// -------------------- Globals --------------------
volatile int32_t encCount = 0;
volatile bool btn1Pressed = false;
volatile bool btn2Pressed = false;
volatile bool encSwPressed = false;

volatile int potRaw = 0;  // <-- volatile (shared antar core)
volatile int servoDeg = 0;
volatile int buzzerFreq = 0;

volatile bool led1 = false, led2 = false, led3 = false; // <-- volatile

const int LEDC_CH = 0;
const int LEDC_RES = 10;      // 10-bit
const int LEDC_BASE_FREQ = 2000;

// -------------------- Encoder ISR --------------------
// FUNGSI fastRead() DIHAPUS - tidak kompatibel S3 & tidak dipakai

void IRAM_ATTR isrEncA() {
  static uint8_t last = 0;
  uint8_t a = digitalRead(PIN_ENC_A);
  uint8_t b = digitalRead(PIN_ENC_B);
  uint8_t state = (a << 1) | b;
  uint8_t combined = (last << 2) | state;

  // Dibalik arah CW/CCW
  if (combined == 0b0001 || combined == 0b0111 || combined == 0b1110 || combined == 0b1000) encCount--;
  else if (combined == 0b0010 || combined == 0b0100 || combined == 0b1101 || combined == 0b1011) encCount++;
  
  last = state;
}

void IRAM_ATTR isrEncSw() { encSwPressed = true; }

void IRAM_ATTR isrBtn1() { btn1Pressed = true; }
void IRAM_ATTR isrBtn2() { btn2Pressed = true; }

// -------------------- Tasks --------------------
void taskIOCore0(void *pv) {
  // Core 0: baca input (pot/encoder/tombol) + kendali stepper
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);
  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);

  attachInterrupt(PIN_ENC_A, isrEncA, CHANGE);
  attachInterrupt(PIN_ENC_SW, isrEncSw, FALLING);
  attachInterrupt(PIN_BTN1, isrBtn1, FALLING);
  attachInterrupt(PIN_BTN2, isrBtn2, FALLING);

  // Stepper setup
  stepper.setMaxSpeed(1200);
  stepper.setAcceleration(600);

  int32_t lastEnc = encCount;
  long target = 0;

  for (;;) {
    // Baca pot (0..4095)
    potRaw = analogRead(PIN_POT);

    // Ubah encCount menjadi target stepper (setiap 4 tick = 1 langkah)
    // Ubah encCount menjadi target stepper (setiap 4 tick = 1 langkah)
    int32_t now = encCount;
    if (now != lastEnc) {
      long delta = (now - lastEnc);
      // CW: delta positif -> target bertambah, CCW: delta negatif -> target berkurang
      target += delta * 4;
      stepper.moveTo(target);
      lastEnc = now;
    }
    stepper.run(); // non-blocking

    // Debounce tombol via flag dari ISR
    if (btn1Pressed) {
      btn1Pressed = false;
      led1 = !led1;
      digitalWrite(PIN_LED1, led1);
      vTaskDelay(pdMS_TO_TICKS(20));
    }
    if (btn2Pressed) {
      btn2Pressed = false;
      led2 = !led2;
      digitalWrite(PIN_LED2, led2);
      vTaskDelay(pdMS_TO_TICKS(20));
    }
    if (encSwPressed) {
      encSwPressed = false;
      led3 = !led3;
      digitalWrite(PIN_LED3, led3);
      vTaskDelay(pdMS_TO_TICKS(20));
    }

    vTaskDelay(pdMS_TO_TICKS(2));
  }
}

void taskActuatorsCore1(void *pv) {
  // Core 1: LED efek, buzzer PWM, servo, OLED
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);

  // Buzzer PWM
  // Use Arduino-ESP32 LEDC API
  ledcSetup(LEDC_CH, LEDC_BASE_FREQ, LEDC_RES);
  ledcAttachPin(PIN_BUZZER, LEDC_CH);

  // Servo
  servo1.attach(PIN_SERVO, 500, 2400);

  // OLED
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  uint32_t t = millis();

  for (;;) {
    // Pot → servo & buzzer
    int raw = potRaw; // Ambil nilai volatile
    servoDeg = map(raw, 0, 4095, 0, 180);
    servo1.write(servoDeg);

    // frekuensi 200..3000 Hz
    buzzerFreq = map(raw, 0, 4095, 200, 3000);
    ledcWriteTone(LEDC_CH, buzzerFreq);
    // duty (on) ~ 40%
    ledcWrite(LEDC_CH, (1 << LEDC_RES) * 0.4);

    // LED3 toggle otomatis setiap 500 ms (selain toggle dari tombol encoder)
    if (millis() - t > 500) {
      t = millis();
      digitalWrite(PIN_LED3, !digitalRead(PIN_LED3));
    }

    // OLED status
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("ESP32-S3 RTOS (2-core)"));
    display.print(F("POT : ")); display.println(raw);
    display.print(F("Servo: ")); display.print(servoDeg); display.println(F(" deg"));
    display.print(F("Buzz : ")); display.print(buzzerFreq); display.println(F(" Hz"));
    display.print(F("Enc  : ")); display.println((int32_t)encCount);
    display.print(F("Step : ")); display.println((long)stepper.currentPosition());
    display.print(F("LEDs : "));
    display.print(led1); display.print(' ');
    display.print(led2); display.print(' ');
    display.println((int)digitalRead(PIN_LED3));
    display.display();

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// -------------------- Setup/Loop --------------------
void setup() {
  // ADC & pins
  analogReadResolution(12);

  // Buat task di dua core
  xTaskCreatePinnedToCore(taskIOCore0, "IO+Stepper", 8192, nullptr, 2, nullptr, 0);
  xTaskCreatePinnedToCore(taskActuatorsCore1, "Actuators+UI", 8192, nullptr, 1, nullptr, 1);
}

void loop() {
  // tidak dipakai (semua via task)
  vTaskDelay(pdMS_TO_TICKS(1000));
}