# Arduino UNO R4 WiFi

## CLI-sourced build data

Pulled via `arduino-cli board details -b arduino:renesas_uno:unor4wifi --json` and `arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi --show-properties`.

- **FQBN:** `arduino:renesas_uno:unor4wifi`
- **Platform to install:** `arduino:renesas_uno` (`arduino-cli core install arduino:renesas_uno`)
- **USB identification:** vid=`0x2341`, pid=`0x1002` (also `0x006D` in bootloader mode)
- **Architecture / MCU:** `cortex-m4` (`build.arch=RENESAS_UNO`, `build.mcu=cortex-m4`)
- **Clock:** 48 MHz (`-DF_CPU=48000000`)
- **Build defines:** `-DF_CPU=48000000 -DNO_USB -DBACKTRACE_SUPPORT -DARDUINO_UNOR4_WIFI`
- **Toolchain:** `arm-none-eabi-gcc` 7-2017q4, `-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16`
- **Upload tool/protocol:** `bossac`, protocol `sam-ba`, native USB, 1200bps-touch reset required
- **Upload size limits:** max sketch (flash) size **262144 bytes** (256 KB), max data (RAM) size **32768 bytes** (32 KB)

Compile/upload commands for this board:
```bash
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi <sketch-dir>
arduino-cli upload -p <port> --fqbn arduino:renesas_uno:unor4wifi <sketch-dir>
```

## Electrical specs (web-sourced)

The CLI doesn't expose electrical/pin specs — sourced from Arduino's own docs and community pinout references.

Also has an **ESP32-S3** co-processor onboard (WiFi + Bluetooth + LED matrix driver) — separate from the RA4M1 above, 3.3V logic.

**Logic level: 5V** on the main RA4M1 header pins. The ESP32-S3 side (and its dedicated 2×3 header) is **3.3V** — do not mix these up when wiring.

### Current limits (RA4M1 GPIO)
- **8 mA max per digital pin**
- **60 mA max combined across all digital pins**
- Never drive a motor, relay coil, or high-power LED directly from a GPIO — always use a transistor/MOSFET/driver IC or a relay module.

### Power
| Pin | Spec |
|---|---|
| USB-C | 5V in, ~4.7V regulated typical |
| VIN | 6–24V (via barrel jack or VIN pin), regulated down to 5V on-board |
| 5V pin | 5V output — ~1.2A via VIN/barrel, ~2A via USB |
| 3.3V pin | 3.3V output, limited current — shared rail also powers the ESP32-S3, so leave headroom during WiFi activity bursts |
| OFF pin | Pull low to cut power — only works when powered via VIN/barrel, not USB |
| VRTC | Backup power for the real-time clock, accepts 1.6–3.6V |

### Pin map

**Digital pins (D0–D13):** 14 total, 5V logic.
- PWM-capable: D3, D5, D6, D9, D10, D11
- External interrupt pins: D2, D3
- D0 (RX), D1 (TX) — hardware UART, exposed as `Serial1`.

**Analog pins (A0–A5):** ADC input, 10-bit default, selectable up to 14-bit. Can double as digital I/O.
- A0 — also **DAC output** (8-bit default, up to 12-bit)
- A1 — Op-amp non-inverting (+) input
- A2 — Op-amp inverting (−) input
- A3 — Op-amp output (max ~4.7V)

**Communication buses:**
| Bus | Pins | Notes |
|---|---|---|
| I2C (5V) | A4 = SDA, A5 = SCL | No built-in pull-ups — add external ones (typ. 4.7kΩ) if a module doesn't include them |
| I2C (3.3V) | Qwiic / STEMMA QT connector | Separate secondary bus, plug-and-play |
| SPI | D10 = CS, D11 = COPI (MOSI), D12 = CIPO (MISO), D13 = SCK | ICSP header also breaks these out |
| CAN | D4 = TX, D5 = RX | Needs an external CAN transceiver module |
| ESP32-S3 header | 2×3 header: GPIO42, GPIO41, GPIO43, GPIO0, GPIO44, GND | All 3.3V logic |

**Other onboard features:**
- 12×8 LED matrix (96 red LEDs, Charlieplexed) — built in, no wiring needed.
- Capacitive touch on: D0, D1, D2, D3, D8, D9, D11, D13, A1, A2, plus a dedicated `LOVE_BUTTON` pad.

### Common beginner gotchas on this board
- Not the same chip family as classic Uno (ATmega328P/AVR) — very old AVR-register-level code won't port directly, though standard Arduino API sketches and libraries work the same.
- WiFi (ESP32-S3) needs its own library (`WiFiS3`) — not the same as ESP32-devkit `WiFi.h` examples found online.
- 5V logic on the main headers means many 3.3V-only sensor breakouts need a level shifter — check the sensor's datasheet before wiring directly.
