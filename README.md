📘 Dokumentasi RTOS ESP32-S3

Proyek ini merupakan dokumentasi singkat mengenai cara **menghubungkan dan mengoperasikan berbagai peripheral pada ESP32-S3 (dual-core)** menggunakan **FreeRTOS** untuk menjalankan beberapa tugas secara paralel.

---

Perangkat yang digunakan:

* 3 × LED indikator
* 2 × Push button
* 1 × Buzzer (aktif/pasif)
* 1 × OLED Display (I2C, resolusi 128×64, alamat 0x3C)
* 1 × Potensiometer (input analog/ADC)
* 1 × Rotary Encoder (sinyal A, B, dan tombol push)
* 1 × Motor Stepper (dengan driver STEP/DIR)
* 1 × Servo Motor (PWM control)

---

Konfigurasi Pin (dapat dilihat juga `diagram.json`)

| Komponen             | Pin ESP32-S3               | 
| -------------------- | -------------------------- | 
| **LED1**             | GPIO2                      | 
| **LED2**             | GPIO4                      | 
| **LED3**             | GPIO5                      |
| **Button 1**         | GPIO0                      | 
| **Button 2**         | GPIO15                     | 
| **Buzzer (PWM)**     | GPIO13                     | 
| **OLED (I2C)**       | SDA = GPIO21, SCL = GPIO22 | 
| **Potensiometer**    | GPIO36                     | 
| **Encoder A**        | GPIO18                     | 
| **Encoder B**        | GPIO19                     | 
| **Encoder Button**   | GPIO23                     | 
| **Stepper (STEP)**   | GPIO26                     | 
| **Stepper (DIR)**    | GPIO25                     | 
| **Stepper (ENABLE)** | GPIO27                     | 
| **Servo (PWM)**      | GPIO14                     | 

<img width="1262" height="756" alt="image" src="https://github.com/user-attachments/assets/dad79137-110a-4e81-b701-a5396c7ea39b" />

---

Struktur Program

Kode utama dapat ditemukan pada:
📁 `src/main.cpp`

Program ini terdiri dari dua task utama yang dijalankan pada core terpisah menggunakan **FreeRTOS**:

* **Core 0:** menangani pembacaan input (tombol, encoder, potensiometer) serta kendali motor stepper.
* **Core 1:** mengatur aktuator seperti LED, buzzer, servo, dan tampilan OLED.

---

Hasil Video Demo


https://github.com/user-attachments/assets/2d4e173f-0a32-4ee5-bd27-09c1ed7ef186



