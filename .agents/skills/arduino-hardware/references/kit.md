# Workshop kit: ELEGOO UNO R4 WiFi Super Starter Kit (EL-KIT-0025)

Every student has exactly this kit. When a project needs a part, **pick it from this list instead of asking the student to identify it.** Ask only when the kit has more than one candidate (e.g. "a display" could be the OLED, a 7-segment display, or the board's own LED grid; "a motor" could be the servo, the stepper, or the fan motor), or when the student says they're using something that isn't in the kit — then fall back to SKILL.md Project work Step 2's normal questions and web lookup.

Source: ELEGOO's component guide for this kit (36 items). Notes marked **(check on the part)** are the usual spec for that part but weren't printed in the guide — confirm against the label/silkscreen on the student's part before relying on them. Library names are as `arduino-cli lib install` expects them; the install script pre-installs all of them.

The board is ELEGOO's own UNO R4 WiFi (a clone of Arduino's open design), programmed as `arduino:renesas_uno:unor4wifi`. Its electrical limits are in `boards/arduino_renesas_uno_unor4wifi.md` — above all **8 mA max per pin, 60 mA total**, which several parts below run into.

## Default pin plan

Use these unless the project needs otherwise, so parts don't collide:

| Pins | Reserved for |
|---|---|
| A4 (SDA), A5 (SCL) | I2C — OLED (address 0x3C) and MPU-6500 (0x68) can share it |
| D9, D10, D11, D12, D13 | RFID reader — D9 reset + SPI on D10–D13. Free for other uses only when the RFID reader isn't in the project |
| D3 | Servo signal |
| D4, D5, D6, D7 | Stepper driver IN1–IN4 |
| D3, D5, D6, D9, D10, D11 | PWM-capable — LED brightness, motor speed (whichever of these the parts above aren't using) |
| D2, D3 | Interrupt-capable — buttons/sensors that must never be missed (D2 first; D3 is the servo's default) |
| A0–A3 | Analog inputs — potentiometer, joystick, photoresistor, thermistor |
| D0, D1 | Avoid — shared with the board's serial port |

## The Basics

**1. UNO R4 WiFi board** ×1 — see the board doc. Has a 12×8 red LED grid (library `Arduino_LED_Matrix`, built into the core) — the easiest "display" for a first project, no wiring.

**2. USB-A to USB-C cable** ×1 — **a USB-A plug**: many recent laptops (most MacBooks) only have USB-C ports and need a USB-C-to-A adapter or hub. If the student's laptop has no USB-A port, that's the first thing to sort out.

**3. 830 tie-point breadboard** ×1 — each numbered row of 5 holes is connected; the middle gap separates the two halves. The long +/− rails along the edges carry power and ground.

**4. Breadboard jumper wires (male-male)** ×65. **5. Female-to-male Dupont wires** ×10 — for connecting a module's pins straight to the board.

## Power

**6. Power supply module** ×1 (MB102-style **(check on the part)**) — plugs onto the breadboard's power rails. Input: its barrel jack (6.5–12V, e.g. the 9V battery) or its USB port. A jumper on each side selects **3.3V or 5V for that side's rail — check both jumpers before powering anything**, especially 3.3V-only parts. Connect its GND to the board's GND. Use it for the servo, stepper, and fan motor so they don't brown out the board. Never feed its output into the board's 5V pin while the board is also on USB.

**7. 9V battery** ×1 + **8. 9V battery connector (barrel plug)** ×1 — plugs into the board's barrel jack (accepts 6–24V) or the power module. A 9V battery drains fast with motors; fine for running a finished project away from the computer.

## Lights, Displays & Sound

**9. LEDs** ×25 (assorted colors). Long leg = + (anode). Always in series with a resistor. **On this board use 1 kΩ** (brown-black-red): the 220–330 Ω common in online tutorials pushes about 9–14 mA, over the R4's 8 mA per-pin limit.

**10. RGB LEDs** ×2 — 4 legs; usually **common cathode (check on the part)**: the longest leg goes to GND, the other three are red/green/blue, each through its own 1 kΩ resistor to a PWM pin.

**11. 1-digit 7-segment display** ×1 — usually ELEGOO's 5161AS, common cathode **(check the label on its side)**. 8 segment pins (a–g + dot), each needing its own resistor; uses 8 board pins, or 3 via the 74HC595.

**12. 4-digit 7-segment display** ×1 — usually ELEGOO's 5461AS, common cathode, 12 pins **(check the label)**. Driven by multiplexing (one digit at a time, fast). Each digit pin carries the current of every lit segment in that digit — keep segment resistors at 1 kΩ so a digit pin stays near the 8 mA limit; brightness will be modest. Use with the 74HC595 to save pins.

**13. 0.96" OLED display module** ×1 — 128×64 monochrome, SSD1306 controller, I2C, address 0x3C **(check on the part)**. 4 pins: GND, VCC (3.3–5V), SCL → A5, SDA → A4 — **pin order varies between batches, so read the labels**. Library: `Adafruit SSD1306` (pulls in `Adafruit GFX Library`).

**14. Active buzzer & passive buzzer** ×1 each — the **active** one has the sticker ("remove after washing") and just beeps at a fixed pitch when its pin is HIGH. The **passive** one plays notes with `tone(pin, frequency)`. Long leg = +. Both draw more than the 8 mA per-pin limit at full volume; fine for short beeps in a workshop, but for anything that sounds continuously drive it through a PN2222 transistor.

## Controls

**15. Push buttons** ×5 + **10 kΩ potentiometer** ×1.
- Buttons have 4 legs in 2 connected pairs; place across the breadboard's middle gap. Wire one side to a pin, the other to GND, and use `pinMode(pin, INPUT_PULLUP)` — no resistor needed; reads `LOW` when pressed.
- Potentiometer: outer legs to 5V and GND, middle leg to an analog pin; `analogRead` gives 0–1023.

**16. 4×4 membrane keypad** ×1 — 8-wire ribbon: 4 rows + 4 columns (typically pins 1–4 rows, 5–8 columns, left to right with the keys facing you **(check on the part)**). Needs 8 digital pins — clashes with SPI if the RFID reader is also used. Library: `Keypad`.

**17. Joystick module** ×1 — pins GND, +5V, VRx → A0, VRy → A1, SW (press) → a digital pin with `INPUT_PULLUP`. Centered reads about 512 on each axis.

**18. IR remote control** ×1 — ELEGOO remote, NEC protocol. Pull out the plastic battery tab first. Each button's code is learned by running IRremote's receive example and pressing buttons — record them here once known.

**19. IR receiver module & IR emitter module** ×1 each — 3 pins each (signal, +5V, GND — **read the labels, order varies**). Receiver signal → any digital pin. Library: `IRremote` (version 4.x supports this board; use `#include <IRremote.hpp>` and `IrReceiver` / `IrSender`).

## Sensors

**20. HC-SR04 ultrasonic distance sensor** ×1 — VCC → 5V, Trig → digital pin, Echo → digital pin, GND. 5V part, safe to connect directly on this 5V board. No library needed (`pulseIn` on Echo; distance cm ≈ microseconds / 58). Range about 2–400 cm.

**21. HC-SR501 PIR motion sensor** ×1 — VCC → 5V, OUT → digital pin, GND. Two orange knobs: sensitivity and how long OUT stays HIGH; a jumper picks single/repeat trigger. Needs up to a minute to settle after power-up — false triggers in that time are normal. OUT is a 3.3V signal: usually read fine, but if motion never registers on this 5V board, that's the first suspect — **not yet confirmed on this board**.

**22. RFID reader (RC522) + key fob** ×1 — **3.3V part: power it from the 3.3V pin, never 5V.** SPI wiring: SDA(SS) → D10, SCK → D13, MOSI → D11, MISO → D12, RST → D9, GND → GND, IRQ unconnected. Its data pins are 3.3V logic and this board's are 5V — the classic Uno tutorials connect them directly and it generally works, but it's outside the chip's rating; say so when wiring it. Library: `MFRC522` (its metadata doesn't list this board, so `arduino-cli` warns about compatibility — the warning is expected; it's plain SPI).

**23. DHT11 temperature & humidity module** ×1 — 3 pins: signal, VCC (5V), GND **(read the labels)**. **No external pull-up resistor needed:** this is the 3-pin module version (blue sensor on a small circuit board), which carries its own pull-up resistor on the board — wire signal straight to a digital pin. Online tutorials that add a 10 kΩ resistor are for the bare 4-pin sensor or the DHT22; don't copy that here. (If a student's part is a bare blue sensor with 4 legs and no circuit board, it does need 10 kΩ from signal to 5V.) Read at most once per second (it returns errors if polled faster). ±2 °C, ±5 % humidity — whole numbers only. Library: `DHT sensor library` (pulls in `Adafruit Unified Sensor`), type `DHT11`.

**24. GY-6500 module (MPU-6500 accelerometer + gyroscope)** ×1 — I2C: VCC (module has a 3.3V regulator, accepts 5V **(check on the part)**), GND, SCL → A5, SDA → A4. Address 0x68 (0x69 if AD0 is tied high). Library: `MPU9250_WE` (includes the `MPU6500_WE` class for this chip).

**25. Photoresistors** ×2 — no polarity. One leg to 5V, the other to an analog pin *and* through a 10 kΩ resistor to GND. Brighter light → higher reading.

**26. Thermistor** ×1 — usually a 10 kΩ NTC **(check on the part)**. Same divider as the photoresistor (10 kΩ to GND); hotter → higher reading with that wiring. Convert with the Beta equation (B ≈ 3950 for typical 10 kΩ NTCs). For real temperatures the DHT11 is simpler.

**27. Tilt ball switch** ×1 — a switch that closes when upright. One leg to a pin with `INPUT_PULLUP`, other to GND.

## Motors & Movement

**28. SG90 servo** ×1 — wires: brown = GND, red = 5V, orange = signal → D3 by default (the `Servo` library works on any digital pin on this board; D3 keeps clear of the RFID reader and stepper). Turns to 0–180°. Library: `Servo`. Current spikes when it moves can reset the board on USB power — if it jitters or the board restarts, power it from the power supply module (shared GND).

**29. 28BYJ-48 stepper motor (5V)** ×1 + **30. ULN2003 driver board** ×1 — the motor's white plug only fits the driver one way. Driver IN1–IN4 → D4–D7 (not the D8–D11 of most online tutorials — those collide with the RFID reader's pins), + → 5V (power supply module recommended), − → GND. About 2048 steps per turn; max ~15 RPM. Library `Stepper` — note the pin order quirk: `Stepper(2048, 4, 6, 5, 7)` (IN1, IN3, IN2, IN4). `AccelStepper` (`FULL4WIRE`, same order) for smooth acceleration.

**31. Fan blade + 3–6V DC motor** ×1 — **never connect to a board pin directly.** Drive it through the L293D (speed + direction) or a PN2222 transistor (on/off/speed): transistor base through 1 kΩ to a PWM pin, motor between + supply and collector, emitter to GND, diode across the motor (stripe toward +). Power the motor from the power supply module.

**32. 5V relay module** ×1 — SRD-05VDC-SL-C relay on a module with 3 pins (signal, +5V, GND) and screw terminals COM/NO/NC. The module includes its own driver, so the signal pin is safe to drive directly. **Workshop rule: low-voltage DC only — never anything plugged into a wall outlet.**

## Small Parts & Helper Chips

**33. Resistors** ×120 (assorted) — value is printed on the paper strip; on loose resistors read the color bands (1 kΩ = brown-black-red, 10 kΩ = brown-black-orange, 220 Ω = red-red-brown).

**34. Rectifier diodes** ×2 (usually 1N4007 **(check on the part)**) — the stripe is the cathode. Across motors and relay coils, stripe toward +, to absorb the voltage kick when they switch off.

**35. PN2222 NPN transistors** ×2 — flat face toward you, legs down: **E, B, C** (left to right). Base always through a resistor (1 kΩ). Switches a few hundred mA — motors, buzzers, LED strips.

**36. 74HC595 shift register** ×1 + **L293D motor driver** ×1 — 16-pin chips; the notch marks pin 1's end (pin 1 is left of the notch, counting down that side and back up the other).
- **74HC595:** pin 16 VCC (5V), pin 8 GND, pin 14 data (DS), pin 11 clock (SHCP), pin 12 latch (STCP), pin 13 (OE) → GND, pin 10 (MR) → 5V. Outputs Q0 = pin 15, Q1–Q7 = pins 1–7. Use `shiftOut()`.
- **L293D:** pin 16 logic 5V, pin 8 motor supply (from the power module), pins 4/5/12/13 GND. Motor A: enable pin 1 (PWM for speed), inputs pins 2 and 7 (direction), outputs pins 3 and 6. Motor B: enable 9, inputs 10/15, outputs 11/14. Has built-in protection diodes.
