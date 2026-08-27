---
name: arduino-hardware
description: "Use whenever the user wants to start, continue, or debug an Arduino/Arduino-compatible hardware project — any board, not a specific one. Triggers: Arduino, arduino-cli, sketch/.ino, breadboard, wiring, pinout, GPIO, sensor, actuator, module, upload/flash to board, 'my Arduino isn't working', 'let's build a project with my board'. The people using this are assumed to be non-technical about hardware — explain things plainly, never assume they know pin voltages or how to avoid frying a board. Never assume which board they have — always detect it."
---

# Arduino Hardware Skill

Board-agnostic guide for Arduino hardware projects: confirm the board is connected before doing anything board-specific, pull everything `arduino-cli` can tell you about it into a reusable context document, find out what modules/components the project needs, then keep compiling, wiring, and safety grounded in real data instead of assumptions.

The user is explicitly non-technical about electronics. Never skip a step to "save time" — the questions and explanations are the point of this skill.

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

## Step 1 — Before touching anything board-specific, ask if it's plugged in

The **first thing** to do when a user wants to start a new hardware project (before writing any board-specific code, before guessing what board they have) is ask, using `AskUserQuestion`:

> "Is your Arduino board plugged into this computer via USB right now?"

with options along the lines of:
- **Yes, it's plugged in**
- **No, I can't plug it in right now**

Don't guess the board or skip this — detection requires a live USB connection, and guessing wrong leads to wrong compile commands and wrong pin/voltage info later.

- **If "No":** that's fine. Say you'll need it plugged in before you can detect the exact board, pull its real specs, and give correct compile commands — everything board-specific waits until then. You can still discuss the project idea and modules in general terms, but don't state specific pin numbers, voltages, or FQBNs until Step 2 has actually run. Ask the user to let you know once it's connected, then stop and wait for their reply — don't proceed further in this turn.
- **If "Yes":** go straight to Step 2.

## Step 2 — Detect the board and build its context document

```bash
arduino-cli board list
```

- If a board is listed with a real **FQBN** (e.g. `arduino:renesas_uno:unor4wifi`), that's your board.
- If it shows **"Unknown"** with only a VID/PID, the matching core isn't installed yet:
  ```bash
  arduino-cli board search <keyword, e.g. the board name if the user knows it>
  arduino-cli core install <platform id, e.g. "arduino:renesas_uno">
  ```
- If nothing shows up at all: the board isn't being detected — often a charge-only USB cable with no data lines, or it's not plugged in all the way. Explain this in plain language before troubleshooting further.

### Pull everything the CLI knows, into a context document

Once you have the FQBN, check `references/boards/` for a file named after it (colons/slashes → underscores, e.g. `arduino:renesas_uno:unor4wifi` → `arduino_renesas_uno_unor4wifi.md`).

- **Exists already:** reuse it for build properties (those don't change), but always re-run `arduino-cli board list` for the current **port** — it can change between sessions/USB ports and is never safe to reuse from a stale doc.
- **Doesn't exist:** generate it now, from real CLI output, not from memory or assumption:
  ```bash
  arduino-cli board details -b <fqbn> --json
  arduino-cli compile --fqbn <fqbn> --show-properties <any-sketch-dir>
  ```
  (If no sketch exists yet for this project, `arduino-cli sketch new` a throwaway one first just to run `--show-properties` against — `--show-properties` needs a sketch path even though it doesn't actually compile anything meaningful for this purpose.)

  `--show-properties` is the important one — it dumps the real MCU, clock speed, compiler flags/defines, toolchain paths, and upload tool/protocol straight from the installed core's own build config. This is exactly what compile commands for this board are built from, so it belongs in the doc verbatim (trimmed to the parts that matter), not paraphrased.

  Write the result to `references/boards/<fqbn_safe>.md` with two sections:
  1. **CLI-sourced build data** — FQBN, MCU/arch, clock speed, memory-relevant defines, upload protocol/tool, and the exact `compile`/`upload` command for this board. All pulled straight from the commands above.
  2. **Electrical specs (web-sourced)** — operating/logic voltage, per-pin current limits, and pin map (digital/analog/PWM/I2C/SPI/etc). The CLI does **not** expose this — look it up (`WebSearch`/`WebFetch` for "`<board name>` pinout specifications") and cite what you found. This section is what Step 5 (wiring) depends on, so don't skip it just because the CLI already gave you plenty of data — voltage/current info is safety-critical and comes from a different source.

  This is how the skill's board knowledge grows over time — every new board encountered gets written back here so future projects on the same board skip straight to reuse.

Before writing any project code, summarize the key facts for the user in plain language: what board it is, its logic voltage, and anything that could get damaged by a wiring mistake.

## Step 3 — Ask what modules/components the project uses

Once the board is confirmed, ask (via `AskUserQuestion`, multi-select) what kind of modules/components this project involves, e.g.:
- WiFi / Bluetooth
- Sensors (temperature, motion, light, distance, etc.)
- Display (LCD, OLED, LED matrix)
- Motors / servos
- Relays / high-power switching
- Audio (speaker, buzzer, mic)
- Not sure yet / none of these

The tool automatically offers an "Other" choice for free text — tell the user in the question that they can use it to paste a product link or describe something not listed, if what they're using isn't one of the categories above.

For each module named or linked, look up its datasheet/pinout/voltage the same way as the board itself (Step 2's "electrical specs" approach) before it's wired in — this feeds directly into Step 5.

## Step 4 — Compile / upload workflow

See [`references/cli-cheatsheet.md`](references/cli-cheatsheet.md) for the full command reference (sketches, libraries, monitor). Core loop, using the FQBN and port from Step 2 (re-fetch the port fresh each time, never reuse an old one):

```bash
arduino-cli sketch new <name>                                   # scaffold a new sketch
arduino-cli compile --fqbn <fqbn> <sketch-dir>                  # compile only
arduino-cli upload -p <port> --fqbn <fqbn> <sketch-dir>         # upload only
arduino-cli compile --upload -p <port> --fqbn <fqbn> <sketch-dir>  # both in one shot
arduino-cli monitor -p <port> -c baudrate=115200                # read Serial output
```

If compile fails on a missing library:
```bash
arduino-cli lib search <keyword>
arduino-cli lib install "<Library Name>"
```

## Step 5 — Always show pinout / wiring info before touching a new component

Every time a project introduces a new sensor, actuator, or module, before telling the user to connect a single wire:

1. Print the relevant pin table from the board's context doc (which pins are free, which are PWM/analog/I2C/etc., voltage) plus the module's own pinout from Step 3.
2. Print a plain-text wiring map, one line per wire, in the form `<component pin> → <board pin>` — e.g.:
   ```
   DHT22 VCC  → 5V
   DHT22 GND  → GND
   DHT22 DATA → D2  (add a 10kΩ pull-up resistor to 5V)
   ```
3. Call out any voltage mismatch explicitly (e.g. a 3.3V-only sensor on a 5V board needs a level shifter or the 3.3V rail, never the 5V pin).
4. If the user wants a visual (not just text) wiring diagram, generate one as an SVG and publish it via the `Artifact` tool rather than trying to draw it in chat — reference `artifact-diagramming` skill guidance for how to make it legible. Default to the plain-text table above when a quick visual isn't specifically asked for; it's more reliable across every surface Claude runs in.

Never wire anything before this step, even if the user seems experienced — assume they aren't.

## Step 6 — Safety rules (always in effect)

See [`references/wiring-safety.md`](references/wiring-safety.md) for the full list. The non-negotiables:

- Never connect anything while the board is powered unless the user explicitly knows what "hot-plugging" risk means for that specific part.
- Double-check polarity (VCC/GND) and voltage rating *before* power is applied, not after something smells hot.
- Respect per-pin and total current limits (see the board's context doc) — don't drive motors/LEDs directly off a GPIO pin without a driver/transistor/resistor as appropriate.
- If a board or component gets hot, smells burnt, or shows smoke: disconnect power immediately and stop — don't troubleshoot live.
