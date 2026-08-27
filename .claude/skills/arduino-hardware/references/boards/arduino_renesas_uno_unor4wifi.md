# Arduino UNO R4 WiFi

Ships in the **ELEGOO UNO R4 WiFi Super Starter Kit** — this repo's default/primary board.

- **FQBN:** `arduino:renesas_uno:unor4wifi`
- **Platform to install:** `arduino:renesas_uno` (`arduino-cli core install arduino:renesas_uno`)
- **USB identification:** vid=`0x2341`, pid=`0x1002` (also `0x006D` in bootloader mode)
- Same physical form factor and pin layout as the classic UNO R3 — shields and diagrams for "Arduino Uno" mostly still apply.

## Core chips

| Chip | Role |
|---|---|
| Renesas RA4M1 (32-bit Arm Cortex-M4 @ 48MHz) | Main microcontroller — runs the sketch |
| ESP32-S3 | Co-processor for WiFi + Bluetooth, plus the onboard LED matrix driver |

## Memory

- Flash: 256 KB
- RAM: 32 KB
- EEPROM: 8 KB

## Power

| Pin | Spec |
|---|---|
| USB-C | 5V in, ~4.7V regulated typical |
| VIN | 6–24V (via barrel jack or VIN pin), regulated down to 5V on-board |
| 5V pin | 5V output — ~1.2A via VIN/barrel, ~2A via USB |
| 3.3V pin | 3.3V output, limited current — shared rail also powers the ESP32-S3, so leave headroom during WiFi activity bursts |
| OFF pin | Pull low to cut power — only works when powered via VIN/barrel, not USB |
| VRTC | Backup power for the real-time clock, accepts 1.6–3.6V |

**Logic level: 5V** on the main RA4M1 header pins. The ESP32-S3 side (and its dedicated 2×3 header) is **3.3V** — do not mix these up when wiring.

## Current limits (RA4M1 GPIO)

- **8 mA max per digital pin**
- **60 mA max combined across all digital pins**
- Never drive a motor, relay coil, or high-power LED directly from a GPIO — always use a transistor/MOSFET/driver IC or a relay module.

## Pin map

### Digital pins (D0–D13)
- 14 total digital I/O pins, 5V logic.
- **PWM-capable:** D3, D5, D6, D9, D10, D11
- **External interrupt pins:** D2, D3
- D0 (RX), D1 (TX) — hardware UART, exposed as `Serial1`. Avoid using these for anything else if the sketch uses Serial1.

### Analog pins (A0–A5)
- ADC input, 10-bit by default, selectable up to 14-bit resolution in software.
- Can also be used as digital I/O.
- **A0** — doubles as **DAC output** (8-bit default, up to 12-bit).
- **A1** — Op-amp non-inverting (+) input.
- **A2** — Op-amp inverting (−) input.
- **A3** — Op-amp output (max ~4.7V).

### Communication buses
| Bus | Pins | Notes |
|---|---|---|
| I2C (5V) | A4 = SDA, A5 = SCL | No built-in pull-up resistors — add external ones (typ. 4.7kΩ) if a module doesn't include them |
| I2C (3.3V) | Qwiic / STEMMA QT connector | Separate secondary bus, plug-and-play, no soldering |
| SPI | D10 = CS, D11 = COPI (MOSI), D12 = CIPO (MISO), D13 = SCK | ICSP header also breaks these out |
| CAN | D4 = TX, D5 = RX | Needs an external CAN transceiver module — not usable standalone |
| ESP32-S3 header | 2×3 header: GPIO42, GPIO41, GPIO43, GPIO0, GPIO44, GND | All 3.3V logic — separate from the main 5V header |

### Other onboard features
- **12×8 LED matrix** (96 red LEDs, Charlieplexed) — built in, no wiring needed.
- **Capacitive touch** on: D0, D1, D2, D3, D8, D9, D11, D13, A1, A2, plus a dedicated `LOVE_BUTTON` pad.

## Compile/upload quick reference

```bash
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi <sketch-dir>
arduino-cli upload -p <port> --fqbn arduino:renesas_uno:unor4wifi <sketch-dir>
```

## Common beginner gotchas on this board

- It's **not** the same chip family as classic Uno (ATmega328P/AVR) — some very old AVR-register-level example code won't port directly, though standard Arduino API sketches (`digitalWrite`, `analogRead`, libraries, etc.) work the same.
- WiFi (ESP32-S3) needs its own set of libraries (`WiFiS3`) — not the same as ESP32-devkit `WiFi.h` examples found online.
- 5V logic on the main headers means many 3.3V-only sensor breakouts (some GPS/BLE/certain camera modules) need a level shifter — check the sensor's datasheet before wiring directly.
