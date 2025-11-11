**Task 2 — Kontrol Peripheral Dual-Core ESP32**

**Tujuan**

* Menghubungkan semua peripheral ke satu board ESP32-S3 DevKit.
* Membuat task/program terpisah untuk setiap peripheral.
* Menjalankan setiap I/O pada core berbeda menggunakan fitur dual-core ESP32 (Core 0 & Core 1).
* Mencatat langkah-langkah percobaan dengan dokumentasi berupa gambar dan video, kemudian mempublikasikan hasilnya di GitHub.

---

**Peripheral yang Digunakan**

| No | Peripheral                   | Fungsi                         | Tipe   | Core   |
| -- | ---------------------------- | ------------------------------ | ------ | ------ |
| 1  | OLED SSD1306 (I²C 128×64)    | Menampilkan teks               | Output | Core 1 |
| 2  | LED                          | Indikator digital (nyala/mati) | Output | Core 0 |
| 3  | Push Button                  | Input digital (tombol tekan)   | Input  | Core 1 |
| 4  | Buzzer                       | Indikator suara                | Output | Core 1 |
| 5  | Potensiometer                | Input analog (ADC)             | Input  | Core 1 |
| 6  | Motor Stepper + Driver A4988 | Gerakan presisi motor          | Output | Core 1 |
| 7  | Rotary Encoder               | Membaca posisi/sudut           | Input  | Core 0 |
| 8  | Servo Motor                  | Gerakan sudut                  | Output | Core 0 |

---

**1. Persiapan Alat dan Bahan**

**Hardware**

* ESP32-S3 DevKitC-1
* LED
* Push Button
* Buzzer
* Potensiometer
* OLED SSD1306 (I²C 128×64)
* Motor Stepper + Driver A4988
* Rotary Encoder
* Servo Motor

**Software**

* Visual Studio Code dengan PlatformIO
* Wokwi Simulator (untuk pengujian virtual)

---

**2. Langkah Percobaan dan Hasil**

A. Build dan Upload Program

1. Buka VSCode dengan PlatformIO dan Wokwi Simulator.
2. Pilih board: ESP32-S3-DevKitC-1.
3. Pastikan struktur folder proyek seperti berikut:

```
.pio
.vscode
/include
/lib
/src/main.cpp       → program utama
/wokwi.toml         → konfigurasi simulasi
/diagram.json       → konfigurasi wiring
/platformio.ini     → konfigurasi build/upload/library
```

4. Konfigurasikan wiring pada file `diagram.json`.
5. Tulis kode program di `main.cpp`.
6. Build project menggunakan PlatformIO, lalu jalankan simulasi di Wokwi: `Start Simulation`.

---

B. Wiring Rangkaian & Hasil Pengujian

1. Wiring
<img width="1489" height="1034" alt="Screenshot 2025-11-12 012444" src="https://github.com/user-attachments/assets/9fab3b73-231d-40ff-a1d3-303125e002e0" />

   
2. Hasil Pengujian
   

https://github.com/user-attachments/assets/9849b0e2-8331-4f9c-991a-4a35084db795



