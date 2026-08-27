---
name: arduino-hardware
description: "Use whenever the user wants to build, continue, or debug an Arduino/Arduino-compatible hardware project, wants to set up/detect their board, or says the detected board is wrong (e.g. a Pro Micro clone misidentified as a Leonardo) — any board, not a specific one. Triggers: Arduino, arduino-cli, sketch/.ino, breadboard, wiring, pinout, GPIO, sensor, actuator, module, display, upload/flash to board, 'my Arduino isn't working', 'let's build a project with my board', 'set up my Arduino board', 'that's not the right board'. The people using this are assumed to be non-technical about hardware — explain things plainly, never assume they know pin voltages or how to avoid frying a board. Never assume which board they have — always detect it via the setup flow below before assuming project work can proceed."
---

# Arduino Hardware Skill

Board-agnostic guide for Arduino hardware projects, with three modes:

- **`/arduino-hardware setup`** — one-time (or re-run-on-demand) board detection that builds a reusable context document.
- **`/arduino-hardware specify <board name, product link, or context>`** — the user tells you what board they actually have, overriding/correcting `arduino-cli`'s USB-ID guess. Needed because auto-detection is genuinely unreliable for some boards — e.g. many **Pro Micro** clones intentionally reuse the **Arduino Leonardo**'s USB VID/PID so generic drivers recognize them, so `board list` will confidently report the wrong board.
- **Project work** (default, auto-triggered by general Arduino conversation) — compiling/uploading, clarifying which module/component is meant whenever one comes up, and keeping wiring grounded in real specs.

The user is explicitly non-technical about electronics. Never skip a step, question, or safety explanation to "save time" — that's the point of this skill.

## What to do when invoked

- User typed `/arduino-hardware setup`, or explicitly asked to set up / detect / re-detect / connect their board → **Setup mode**, stop there.
- User typed `/arduino-hardware specify ...`, or said the detected/assumed board is wrong and told you what it actually is → **Specify mode**, stop there.
- Otherwise → general project conversation, go to **Project work** below.

---

## Setup mode (`/arduino-hardware setup`)

### Step 1 — Make sure `arduino-cli` is installed

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

Explain in one sentence what `arduino-cli` is ("the official command-line tool that compiles your code and sends it to the board") — don't assume the user knows.

### Step 2 — Ask if the board is plugged in

Before running any detection command, ask using `AskUserQuestion`:

> "Is your Arduino board plugged into this computer via USB right now?"

with options along the lines of:
- **Yes, it's plugged in**
- **No, I can't plug it in right now**

- **If "No":** explain that detection needs a live USB connection, and that you'll pick this back up once it's connected. Ask them to let you know when it's plugged in, then stop and wait for their reply — don't guess a board or proceed further this turn.
- **If "Yes":** continue to Step 3.

### Step 3 — Detect the board and build its context document

```bash
arduino-cli board list
```

- If a board is listed with a real **FQBN** (e.g. `arduino:renesas_uno:unor4wifi`), that's your board.
- If it shows **"Unknown"** with only a VID/PID, the matching core isn't installed yet:
  ```bash
  arduino-cli board search <keyword, e.g. the board name if the user knows it>
  arduino-cli core install <platform id, e.g. "arduino:renesas_uno">
  ```
- If nothing shows up at all: not a code problem — usually a charge-only USB cable with no data lines, or it isn't fully seated. Explain that in plain language before troubleshooting further, and re-run `board list` after they check the cable.

`board list` is a best guess from USB vendor/product IDs, not a certainty — some clone boards (most notably many **Pro Micro** clones) intentionally share IDs with a *different* official board (usually **Leonardo**) purely so generic drivers recognize them. If the user says the detected board doesn't match what they actually bought, don't argue with them or force the auto-detected FQBN — point them to `/arduino-hardware specify` instead.

Once you have the FQBN, check `references/boards/` for a file named after it (colons/slashes → underscores, e.g. `arduino:renesas_uno:unor4wifi` → `arduino_renesas_uno_unor4wifi.md`).

- **Exists already:** tell the user this board's already set up, confirm the current **port** with a fresh `arduino-cli board list` (ports can change between sessions), and stop — no need to regenerate build data that hasn't changed.
- **Doesn't exist:** generate it now, from real CLI output, not from memory or assumption:
  ```bash
  arduino-cli board details -b <fqbn> --json
  arduino-cli compile --fqbn <fqbn> --show-properties <any-sketch-dir>
  ```
  (If no sketch exists yet, `arduino-cli sketch new` a throwaway one first just to have a path to run `--show-properties` against.)

  `--show-properties` is the important one — it dumps the real MCU, clock speed, compiler flags/defines, toolchain paths, and upload tool/protocol straight from the installed core's own build config. This is exactly what compile commands for this board are built from, so it belongs in the doc verbatim (trimmed to the parts that matter), not paraphrased.

  Write the result to `references/boards/<fqbn_safe>.md` with two sections:
  1. **CLI-sourced build data** — FQBN, MCU/arch, clock speed, memory-relevant defines, upload protocol/tool, upload size limits, and the exact `compile`/`upload` command for this board. All pulled straight from the commands above.
  2. **Electrical specs (web-sourced)** — operating/logic voltage, per-pin current limits, and pin map (digital/analog/PWM/I2C/SPI/etc). The CLI does **not** expose this — look it up (`WebSearch`/`WebFetch` for "`<board name>` pinout specifications") and cite what you found. This section is what wiring work depends on later, so don't skip it just because the CLI already gave you plenty of data — voltage/current info is safety-critical and comes from a different source.

This is how the skill's board knowledge grows over time — every new board encountered gets written back here so future setup runs on the same board just reuse it.

### Step 4 — Confirm and hand off

Summarize for the user in plain language: what board was detected, its logic voltage, and its current port. Let them know they're set up and can go ahead and describe the project they want to build.

---

## Specify mode (`/arduino-hardware specify <board name, product link, or context>`)

Use this whenever `arduino-cli`'s auto-detection doesn't match what the user actually has. This is a real, common failure mode — not hypothetical: boards like SparkFun/generic **Pro Micro** clones deliberately reuse the **Arduino Leonardo**'s USB VID/PID so they boot with existing drivers, so `board list` will confidently report "Leonardo" for a board that behaves differently enough (different pin count, no onboard LED on the same pin, etc.) that a Leonardo-targeted compile can produce working-but-subtly-wrong code. The user's own knowledge of what they bought overrides the CLI's guess here.

### Step 1 — Resolve what the user actually gave you

Whatever follows `specify` — a board name, a product link, or free-text description (e.g. "SparkFun Pro Micro 5V/16MHz", or a link to the exact listing):

- **Link:** `WebFetch` it to identify the exact board/model.
- **Name:** `arduino-cli board search <name>` to find matching FQBNs.
- **Ambiguous** (e.g. multiple Pro Micro variants — 3.3V/8MHz vs 5V/16MHz — which have different clock speeds and matter for timing-sensitive code): ask the user to pick via `AskUserQuestion` rather than guessing.

### Step 2 — Confirm before writing anything

State the resolved board and FQBN in plain language and confirm it's right — e.g. "So this is a SparkFun Pro Micro (5V/16MHz), FQBN `SparkFun:avr:promicro16` — that sound right?" — before touching any files.

### Step 3 — Install the core if needed

```bash
arduino-cli core install <platform id>
```
(Third-party boards like SparkFun's may need their board manager URL added first — check `arduino-cli core search <name>` and, if nothing turns up, ask the user for the board's board-manager JSON URL from its product page/docs, or add it via `arduino-cli config add board_manager.additional_urls <url>`.)

### Step 4 — Build the context document, and mark it as user-confirmed

Same as Setup mode Step 3: pull `arduino-cli board details -b <fqbn> --json` and `arduino-cli compile --fqbn <fqbn> --show-properties <sketch-dir>` for CLI-sourced data, plus a web-sourced electrical-specs section, into `references/boards/<fqbn_safe>.md`. But add a line at the very top of the doc:

> **Confirmed by user, not auto-detected.** `arduino-cli board list` reports this board's USB ID as `<whatever it showed, e.g. "Unknown" or "Arduino Leonardo">` — that detection is wrong/ambiguous for this specific board. This doc is for the real board: `<real name>`.

Keep that note permanently — it's what stops a future session from trusting the CLI's guess again.

### Step 5 — Clean up a stale wrong-board doc, if one exists

If `references/boards/` already has a doc filed under the *auto-detected* (wrong) FQBN from an earlier session — e.g. `arduino_avr_leonardo.md` for what's actually a Pro Micro — don't leave it sitting there to be picked up by mistake later. Tell the user it exists and was likely a misdetection, and delete it (or fold a note into it pointing at the corrected file) rather than silently leaving two conflicting docs around.

### Step 6 — Confirm and hand off

Same as Setup mode Step 4.

---

## Project work (default)

### Step 1 — Confirm setup has run

Check `references/boards/` for at least one board context file. If it's empty (no board has ever been detected in this project), tell the user to run `/arduino-hardware setup` first — don't guess a board's FQBN, voltage, or pin map from scratch here. Once a context doc exists for the board they're using, proceed.

If the user is mid-conversation about a project and setup hasn't run, it's fine to discuss the project idea in general terms, but hold off on specific pin numbers, voltages, or compile commands until setup has produced real data.

### Step 2 — Clarify the exact module whenever one comes up

This is an **ongoing** behavior for the whole conversation, not a one-time upfront question. Whenever the user describes a piece of functionality that implies a hardware module — a display, a sensor, a motor, a wireless connection, a button, anything that isn't already using the board's own onboard I/O — and they haven't already named the specific part, stop and ask before writing code or wiring instructions for it.

Example: user says "I want to display this number on a display." Don't assume which display. Ask something like:

> "What display do you have for this? (e.g. 16x2 character LCD, small OLED like an SSD1306, 7-segment display, or the board's built-in LED matrix if it has one)"

Use `AskUserQuestion` with a few common options for that category of module plus room for "Other" (paste a product link or describe it) when there's a natural short list; ask a plain open question when there isn't. The goal is always to end up with a specific, identifiable part — a category name alone ("a display") isn't enough to give correct pins, voltage, or wiring.

Once the exact module is identified, look up its datasheet/pinout/voltage (`WebSearch`/`WebFetch`) before it's wired in — this feeds directly into Step 4. If the same module was already clarified and looked up earlier in the conversation, don't ask again.

### Step 3 — Compile / upload workflow

See [`references/cli-cheatsheet.md`](references/cli-cheatsheet.md) for the full command reference (sketches, libraries, monitor). Core loop, using the FQBN and port from the board's context doc (re-fetch the port fresh each time with `arduino-cli board list` — never reuse an old one, it can change between sessions):

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

### Step 4 — Always show pinout / wiring info before touching a new component

Every time a project introduces a new sensor, actuator, or module (after it's been clarified per Step 2), before telling the user to connect a single wire:

1. Print the relevant pin table from the board's context doc (which pins are free, which are PWM/analog/I2C/etc., voltage) plus the module's own pinout from Step 2.
2. Print a plain-text wiring map, one line per wire, in the form `<component pin> → <board pin>` — e.g.:
   ```
   DHT22 VCC  → 5V
   DHT22 GND  → GND
   DHT22 DATA → D2  (add a 10kΩ pull-up resistor to 5V)
   ```
3. Call out any voltage mismatch explicitly (e.g. a 3.3V-only sensor on a 5V board needs a level shifter or the 3.3V rail, never the 5V pin).
4. If the user wants a visual (not just text) wiring diagram, generate one as an SVG and publish it via the `Artifact` tool rather than trying to draw it in chat — reference `artifact-diagramming` skill guidance for how to make it legible. Default to the plain-text table above when a quick visual isn't specifically asked for; it's more reliable across every surface Claude runs in.

Never wire anything before this step, even if the user seems experienced — assume they aren't.

### Step 5 — Safety rules (always in effect)

See [`references/wiring-safety.md`](references/wiring-safety.md) for the full list. The non-negotiables:

- Never connect anything while the board is powered unless the user explicitly knows what "hot-plugging" risk means for that specific part.
- Double-check polarity (VCC/GND) and voltage rating *before* power is applied, not after something smells hot.
- Respect per-pin and total current limits (see the board's context doc) — don't drive motors/LEDs directly off a GPIO pin without a driver/transistor/resistor as appropriate.
- If a board or component gets hot, smells burnt, or shows smoke: disconnect power immediately and stop — don't troubleshoot live.
