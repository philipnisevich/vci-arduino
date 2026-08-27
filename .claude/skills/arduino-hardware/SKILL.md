---
name: arduino-hardware
description: "Use whenever the user is working with an Arduino or Arduino-compatible board — starting a new sketch, wiring a sensor/component, compiling or uploading code, or asking what board they have. Triggers: Arduino, arduino-cli, sketch/.ino, UNO R4 WiFi, ELEGOO kit, breadboard, wiring, pinout, GPIO, sensor, actuator, upload/flash to board, 'my Arduino isn't working'. The people using this are assumed to be non-technical about hardware — explain things plainly, never assume they know pin voltages or how to avoid frying a board."
---

# Arduino Hardware Skill

Guides Claude through any Arduino hardware project end-to-end: making sure the tooling exists, identifying the exact board in front of the user, teaching Claude (and the user) that board's specs, and then keeping wiring safe and pinouts visible throughout the project.

**Default board:** this repo's primary kit is the **ELEGOO UNO R4 WiFi Super Starter Kit**, which is an official **Arduino UNO R4 WiFi** (FQBN `arduino:renesas_uno:unor4wifi`). Treat this as the assumed board unless detection (Step 1) says otherwise. Full specs: [`references/boards/arduino_renesas_uno_unor4wifi.md`](references/boards/arduino_renesas_uno_unor4wifi.md).

The user is explicitly non-technical about electronics. Never skip the safety/spec explanation steps to "save time" — that explanation is the point of this skill.

## Step 0 — Make sure `arduino-cli` is installed

```bash
arduino-cli version
```

If that fails (command not found), install it, then re-run `version` to confirm:

| OS | Install command |
|----|------------------|
| macOS | `brew install arduino-cli` |
| Linux | `curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh \| sh` (installs to `./bin`; add to `PATH`) |
| Windows | `winget install ArduinoSA.CLI` (or `choco install arduino-cli`) |

First-time setup after install:
```bash
arduino-cli config init
arduino-cli core update-index
```

Explain to the user in one sentence what `arduino-cli` is ("the official command-line tool that compiles your code and sends it to the board") — don't assume they know.

## Step 1 — Identify the board

Always detect rather than assume, even though the default is the UNO R4 WiFi — the user may plug in something else later.

```bash
arduino-cli board list
```

- If a board is listed with a real **FQBN** (e.g. `arduino:renesas_uno:unor4wifi`), that's your board.
- If it shows **"Unknown"** with only a VID/PID, the matching core isn't installed yet. Search by name and install:
  ```bash
  arduino-cli board search <keyword, e.g. "uno r4">
  arduino-cli core install <platform id, e.g. "arduino:renesas_uno">
  ```
- If nothing shows up at all: the board likely isn't plugged in via USB yet, or needs a driver/cable (common issue: charge-only USB cables have no data lines). Tell the user in plain language to check the cable and connection before anything else.
- Once you have the FQBN, confirm details:
  ```bash
  arduino-cli board details -b <fqbn>
  ```

## Step 2 — Load or build that board's spec sheet, then show it to the user

Check `references/boards/` for a file matching the FQBN (slashes/colons → underscores, e.g. `arduino:renesas_uno:unor4wifi` → `arduino_renesas_uno_unor4wifi.md`).

- **Exists** (true for the default UNO R4 WiFi): read it and summarize the key specs for the user in plain language before writing any code — operating voltage, logic level, memory, and anything that could get damaged by a wiring mistake (e.g. this board's `5V` logic on the main headers vs `3.3V` on the ESP32-S3 side).
- **Doesn't exist** (a new/different board): `arduino-cli board details -b <fqbn>` only gives package/programmer metadata, not electrical specs or pin maps. Use `WebSearch`/`WebFetch` for "`<board name>` pinout specifications", summarize the same categories, and **write a new file** into `references/boards/` following the format of `arduino_renesas_uno_unor4wifi.md` so the skill knows this board next time without re-searching. This is how the skill's board knowledge grows over time — always contribute back.

Never start wiring or coding without having stated: operating/logic voltage, max current per pin, and which pins are special-purpose (PWM, ADC, I2C, SPI, DAC, etc.).

## Step 3 — Compile / upload workflow

See [`references/cli-cheatsheet.md`](references/cli-cheatsheet.md) for the full command reference (sketches, libraries, monitor). Core loop:

```bash
arduino-cli sketch new <name>                                   # scaffold a new sketch
arduino-cli compile --fqbn <fqbn> <sketch-dir>                  # compile only
arduino-cli upload -p <port> --fqbn <fqbn> <sketch-dir>         # upload only
arduino-cli compile --upload -p <port> --fqbn <fqbn> <sketch-dir>  # both in one shot
arduino-cli monitor -p <port> -c baudrate=115200                # read Serial output
```

Get `<port>` from `arduino-cli board list` each time — it can change between USB plug-ins/reboots, especially on macOS/Linux. Don't hardcode it.

If compile fails on a missing library:
```bash
arduino-cli lib search <keyword>
arduino-cli lib install "<Library Name>"
```

## Step 4 — Always show pinout / wiring info before touching a new component

Every time a project introduces a new sensor, actuator, or module, before telling the user to connect a single wire:

1. Print the relevant pin table from the board's reference file (which pins are free, which are PWM/analog/I2C/etc., voltage).
2. Print a plain-text wiring map, one line per wire, in the form `<component pin> → <board pin>` — e.g.:
   ```
   DHT22 VCC  → 5V
   DHT22 GND  → GND
   DHT22 DATA → D2  (add a 10kΩ pull-up resistor to 5V)
   ```
3. Call out any voltage mismatch explicitly (e.g. a 3.3V-only sensor on a 5V board needs a level shifter or the 3.3V rail, never the 5V pin).
4. If the user wants a visual (not just text) wiring diagram, generate one as an SVG and publish it via the `Artifact` tool rather than trying to draw it in chat — reference `artifact-diagramming` skill guidance for how to make it legible. Default to the plain-text table above when a quick visual isn't specifically asked for; it's more reliable across every surface Claude runs in.

Never wire anything before this step, even if the user seems experienced — assume they aren't.

## Step 5 — Safety rules (always in effect)

See [`references/wiring-safety.md`](references/wiring-safety.md) for the full list. The non-negotiables:

- Never connect anything while the board is powered unless the user explicitly knows what "hot-plugging" risk means for that specific part.
- Double-check polarity (VCC/GND) and voltage rating *before* power is applied, not after something smells hot.
- Respect per-pin and total current limits (see the board's reference file) — don't drive motors/LEDs directly off a GPIO pin without a driver/transistor/resistor as appropriate.
- If a board or component gets hot, smells burnt, or shows smoke: disconnect power immediately and stop — don't troubleshoot live.
