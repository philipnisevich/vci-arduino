---
name: arduino-hardware
description: "Use whenever the user wants to build, continue, or debug an Arduino/Arduino-compatible hardware project, wants to install the Arduino tools, wants to check/set up/detect their board, or says the detected board is wrong (e.g. a Pro Micro clone misidentified as a Leonardo) — any board, not a specific one. Triggers: Arduino, arduino-cli, sketch/.ino, breadboard, wiring, pinout, GPIO, sensor, actuator, module, display, upload/flash to board, 'my Arduino isn't working', 'let's build a project with my board', 'install the Arduino stuff', 'check my board', 'set up my Arduino board', 'that's not the right board'. The people using this are assumed to be non-technical about hardware — explain things plainly, never assume they know pin voltages or how to avoid frying a board. Never assume which board they have — always confirm the board (check or setup, below) before assuming project work can proceed."
---

# Arduino Hardware Skill

Board-agnostic guide for Arduino hardware projects, tuned for a workshop that uses the **Arduino UNO R4 WiFi** (FQBN `arduino:renesas_uno:unor4wifi`, reference doc already bundled in `references/boards/`). Modes:

- **`/arduino-hardware install`** — before the workshop, **no board needed**. Installs every tool and download the workshop needs. Never touches a board.
- **`/arduino-hardware check`** — at the workshop, board plugged in. A quick, **strictly read-only** connection check: is the board there, is it the workshop board, which port is it on. **Never flashes or uploads anything** — the board's factory demo animation must still be playing when this finishes.
- **`/arduino-hardware setup [board name, product link, or context]`** — the general flow for any *other* board, or when detection is wrong. The argument is optional; pass a name/link when you already know the board or when `arduino-cli`'s USB-ID guess is wrong (e.g. many **Pro Micro** clones reuse the **Arduino Leonardo**'s USB VID/PID, so auto-detection confidently reports the wrong board) — that info then takes priority over the auto-detected guess.
- **Project work** (default, auto-triggered by general Arduino conversation) — using the workshop's reference code for the demo projects, compiling/uploading, clarifying which module/component is meant whenever one comes up, and keeping wiring grounded in real specs.

The user is explicitly non-technical about electronics *and* about computers. Never skip a step, question, or safety explanation to "save time" — that's the point of this skill. Never ask them to open a terminal or type a command themselves; you run everything.

## What to do when invoked

- `/arduino-hardware install`, or they ask to install / get ready / prepare before the workshop → **Install mode**, stop there.
- `/arduino-hardware check`, or they ask to check / test their board connection → **Check mode**, stop there.
- `/arduino-hardware setup` (with or without a trailing board name/link/context), or they said the detected/assumed board is wrong and told you what it actually is → **Setup mode**, stop there.
- Otherwise → **Project work**.

## Finding the tools

The tools are installed into `~/.local/bin`, which usually isn't on the shell's `PATH`. Every command in this skill written as `arduino-cli` or `arduino-fwuploader` means: use the bare name if `command -v arduino-cli` (resp. `arduino-fwuploader`) finds it, otherwise call it by full path as `~/.local/bin/arduino-cli` (resp. `~/.local/bin/arduino-fwuploader`) — write it exactly like that, with the `~`, so it matches the project's pre-approved commands in `.claude/settings.json`. If neither exists, the tools aren't installed: run Install mode's script first (it doesn't touch the board, so it's always safe), then continue.

## Per-project board record: `.arduino-board.json`

The project root holds `.arduino-board.json` once a board has been confirmed (by Check or Setup mode). This file — **not** the presence of docs in `references/boards/`, which ship with the skill — is what says "this student's board is known." Shape:

```json
{
  "fqbn": "arduino:renesas_uno:unor4wifi",
  "name": "Arduino UNO R4 WiFi",
  "reference": "references/boards/arduino_renesas_uno_unor4wifi.md",
  "source": "auto-detected",
  "confirmed": "2026-11-01",
  "wifi_firmware": null
}
```

- `source` is `"auto-detected"` or `"user-confirmed"` (Setup mode Step 2 overrode the CLI's guess).
- `wifi_firmware` is `null` until Project work's firmware step has read the Wi-Fi chip's firmware version; then it's that version string (e.g. `"0.6.0"`).
- Never store the port here — ports change between sessions; always re-read with `arduino-cli board list`.

---

## Install mode (`/arduino-hardware install`)

Runs with no board present, so students can do it at home before opening the kit.

1. Tell them in one or two plain sentences what's about to happen: "I'm going to download the free Arduino tools — the program that turns your code into something the board understands, plus support for your specific board. It's a few hundred megabytes and can take around 10 minutes. You don't need to plug anything in, and it won't ask for your password." Ask them to keep the computer awake and online.
2. Run the bundled installer, **with the longest Bash timeout available (600000 ms)** — the board-support download alone can take most of 10 minutes:
   ```bash
   bash .agents/skills/arduino-hardware/scripts/install-tools.sh
   ```
   It installs `arduino-cli` and `arduino-fwuploader` into `~/.local/bin` from Arduino's official downloads (no Homebrew, no `sudo`, no password), refreshes the board list, installs the workshop board's core (`arduino:renesas_uno`), installs Rosetta on Apple Silicon Macs (the core's compiler is an Intel-only program), and installs the code libraries for every part in the kit (see `references/kit.md`). Anything already installed is kept, not reinstalled. If macOS pops up a window during this (e.g. asking to allow access to the Documents folder, where libraries are stored), tell them it's expected and to click Allow/OK. Its last two lines print where each tool lives.
3. If it fails or gets cut off by the timeout, just run it again — it skips whatever's already done, and `arduino-cli` keeps partial downloads. If a download fails, it's almost always the internet connection; say so plainly.
4. **Windows:** this script runs in the agent's Git Bash shell and downloads the Windows builds. It hasn't been tested on a workshop Windows machine yet — if anything fails there, explain what happened plainly and stop rather than improvising admin-level fixes.
5. Finish with: "You're all set. Don't open the kit or plug anything in yet — at the workshop we'll run `/arduino-hardware check` when you plug the board in for the first time."

---

## Check mode (`/arduino-hardware check`)

Quick and **read-only**. The board ships with a demo animation on its LED grid, and students should see it the first time they plug in — so:

> **Hard rule for Check mode: never run `upload`, `compile --upload`, `monitor`, `burn-bootloader`, or any `arduino-fwuploader` command, and never open the board's serial port in any way** (even opening it at 1200 baud resets the board into upload mode and stops the animation). The only command that touches the board here is `arduino-cli board list`.

1. **Tools present?** Resolve the tools (see *Finding the tools*). If they're missing, the student skipped Install mode — tell them that's fine, run the installer (Install mode steps 1–3), then continue.
2. **Ask if it's plugged in**, via `AskUserQuestion`:
   > "Plug your Arduino into this computer with the USB-C cable from the kit. Once it's in, the little grid of red lights on the board should start playing an animation. Is it plugged in?"
   - **"Yes, it's plugged in and the lights are on"** → step 3.
   - **"It's plugged in but nothing lights up"** → the board isn't getting power: check the cable is pushed in fully at both ends, try another USB port. Ask again.
   - **"Not yet"** → tell them to let you know once it's in, and stop this turn.
3. **Detect:** `arduino-cli board list --json`.
   - Lists `arduino:renesas_uno:unor4wifi` → it's the workshop board. Use the bundled `references/boards/arduino_renesas_uno_unor4wifi.md` as-is — **no web research, no regenerating it**.
   - The kit's cable has a **USB-A** plug (the wider, rectangular kind). If their laptop only has USB-C ports (most MacBooks), they need a USB-C-to-A adapter or hub — don't let them hunt for a port that isn't there.
   - Nothing listed, but the lights are on → the board has power but no data connection. That's almost always a **charge-only USB cable** (it carries power but not data) or a loose plug. Explain that plainly, have them swap to the kit's cable or another port, and re-run `board list`.
   - Listed as "Unknown" → the board core isn't installed; run the installer, then re-run `board list`.
   - A different board → it isn't the workshop board. Tell them, and switch to **Setup mode** (from Step 4) for that board.
4. **Record it:** write `.arduino-board.json` in the project root (see shape above; `source: "auto-detected"`, today's date, `wifi_firmware: null` unless a previous record already had a value — keep it).
5. **Report** in plain language: "Your Arduino UNO R4 WiFi is connected and ready (port `<port>`). The animation it's playing is a demo that came on the board — it'll keep running until you build your first project, which will replace it." Then invite them to describe what they want to build.

---

## Setup mode (`/arduino-hardware setup [board name, product link, or context]`)

For boards other than the workshop board, or when detection is wrong. Like Check mode, setup only *reads* from the board — it never uploads.

### Step 1 — Make sure the tools are installed

Resolve the tools (see *Finding the tools*). If missing, run the installer in tools-only mode (it still installs Rosetta on Apple Silicon Macs, which every Arduino compiler needs; the right core gets installed in Step 4, once the board is known), with a 600000 ms timeout:
```bash
bash .agents/skills/arduino-hardware/scripts/install-tools.sh --tools-only
```

Explain in one sentence what `arduino-cli` is ("the official tool that turns your code into something the board can run and sends it to the board") — don't assume the user knows.

### Step 2 — If a board name/link/context was given, resolve it now

This step only applies if the user passed something after `setup` (or told you elsewhere in conversation what board they actually have) — **skip straight to Step 3 if they didn't; that's the normal case and nothing here is required.**

When something was given:
- **Link:** `WebFetch` it to identify the exact board/model.
- **Name:** `arduino-cli board search <name>` to find matching FQBNs.
- **Ambiguous** (e.g. multiple Pro Micro variants — 3.3V/8MHz vs 5V/16MHz — which have different clock speeds and matter for timing-sensitive code): ask the user to pick via `AskUserQuestion` rather than guessing.

State the resolved board and FQBN in plain language and confirm it's right — e.g. "So this is a SparkFun Pro Micro (5V/16MHz), FQBN `SparkFun:avr:promicro16` — that sound right?" — before continuing. This resolved FQBN now takes priority over whatever `arduino-cli` guesses from the USB connection in Step 4, and the resulting doc will be marked as user-confirmed rather than auto-detected (Step 5).

(Third-party boards like SparkFun's may need their board manager URL added first — check `arduino-cli core search <name>` and, if nothing turns up, ask the user for the board's board-manager JSON URL from its product page/docs, or add it via `arduino-cli config add board_manager.additional_urls <url>`.)

### Step 3 — Ask if the board is plugged in

Before running any detection command, ask using `AskUserQuestion`:

> "Is your Arduino board plugged into this computer with a USB cable right now?"

with options along the lines of:
- **Yes, it's plugged in**
- **No, I can't plug it in right now**

- **If "No":** explain that reading the board's details needs it connected, and that you'll pick this back up once it's plugged in. If Step 2 already resolved which board this is, say so — that part's done. Ask them to let you know when it's plugged in, then stop and wait for their reply — don't guess further or proceed this turn.
- **If "Yes":** continue to Step 4.

### Step 4 — Detect the board

```bash
arduino-cli board list
```

- If Step 2 already resolved an FQBN from user-supplied context, use that as authoritative — if it conflicts with what `board list` guesses from the USB ID, go with the user-supplied one, not the auto-detected one.
- Otherwise, if a board is listed with a real **FQBN** (e.g. `arduino:renesas_uno:unor4wifi`), that's your board.
- If it shows **"Unknown"** with only a VID/PID, the matching core isn't installed yet:
  ```bash
  arduino-cli board search <keyword, e.g. the board name if the user knows it>
  arduino-cli core install <platform id, e.g. "arduino:renesas_uno">
  ```
- If nothing shows up at all: not a code problem — usually a charge-only USB cable with no data lines, or it isn't fully seated. Explain that in plain language before troubleshooting further, and re-run `board list` after they check the cable.

`board list` is a best guess from USB vendor/product IDs, not a certainty — some clone boards (most notably many **Pro Micro** clones) intentionally share IDs with a *different* official board (usually **Leonardo**) purely so generic drivers recognize them. If the user says the detected board doesn't match what they actually bought, don't argue with them or force the auto-detected FQBN — ask them for the real board name/link (or have them re-run `/arduino-hardware setup <name or link>`), which routes back to Step 2.

Install the core for the resolved FQBN if it isn't installed yet (`arduino-cli core install <platform id>`).

### Step 5 — Build the context document

Once you have the FQBN, check `references/boards/` for a file named after it (colons/slashes → underscores, e.g. `arduino:renesas_uno:unor4wifi` → `arduino_renesas_uno_unor4wifi.md`).

- **Exists already (including docs bundled with the skill), and this run didn't override it with new user-supplied context:** use it as-is — no research, no regenerating. Skip to Step 6.
- **Doesn't exist, or needs to be (re)written because Step 2 resolved/overrode the board:** generate it now, from real CLI output, not from memory or assumption:
  ```bash
  arduino-cli board details -b <fqbn> --json
  arduino-cli compile --fqbn <fqbn> --show-properties <any-sketch-dir>
  ```
  (If no sketch exists yet, `arduino-cli sketch new` a throwaway one first just to have a path to run `--show-properties` against. `compile` alone never touches the board.)

  `--show-properties` is the important one — it dumps the real MCU, clock speed, compiler flags/defines, toolchain paths, and upload tool/protocol straight from the installed core's own build config. This is exactly what compile commands for this board are built from, so it belongs in the doc verbatim (trimmed to the parts that matter), not paraphrased.

  Write the result to `references/boards/<fqbn_safe>.md` with two sections:
  1. **CLI-sourced build data** — FQBN, MCU/arch, clock speed, memory-relevant defines, upload protocol/tool, upload size limits, and the exact `compile`/`upload` command for this board. All pulled straight from the commands above.
  2. **Electrical specs (web-sourced)** — operating/logic voltage, per-pin current limits, and pin map (digital/analog/PWM/I2C/SPI/etc). The CLI does **not** expose this — look it up (`WebSearch`/`WebFetch` for "`<board name>` pinout specifications") and cite what you found. This section is what wiring work depends on later, so don't skip it just because the CLI already gave you plenty of data — voltage/current info is safety-critical and comes from a different source.

  If Step 2 resolved/overrode the board from user-supplied context, add a line at the very top of the doc:
  > **Confirmed by user, not auto-detected.** `arduino-cli board list` reports this board's USB ID as `<whatever it showed, e.g. "Unknown" or "Arduino Leonardo">` — that detection is wrong/ambiguous for this specific board. This doc is for the real board: `<real name>`.

  Keep that note permanently — it's what stops a future session from trusting the CLI's guess again. And if `references/boards/` already has a doc filed under the *wrong*, previously auto-detected FQBN from an earlier session (e.g. `arduino_avr_leonardo.md` for what's actually a Pro Micro), don't leave it sitting there to be picked up by mistake — tell the user it exists and was likely a misdetection, and delete it (or fold a note into it pointing at the corrected file). Never delete a doc bundled with the skill (e.g. the UNO R4 WiFi one).

This is how the skill's board knowledge grows over time — every new board encountered gets written back here so future setup runs on the same board just reuse it.

### Step 6 — Record, confirm, and hand off

Write `.arduino-board.json` (see shape above; `source` is `"user-confirmed"` if Step 2 overrode detection). Then summarize for the user in plain language: what board was detected/confirmed, its logic voltage, and its current port. Let them know they're set up and can go ahead and describe the project they want to build.

---

## Project work (default)

### Step 1 — Confirm the board is known

Read `.arduino-board.json` in the project root. If it's missing, the board hasn't been confirmed yet — run **Check mode** now (it's quick and read-only); if the board turns out not to be the workshop board, Check mode hands off to Setup mode. Don't guess a board's FQBN, voltage, or pin map. Docs existing in `references/boards/` do **not** count — they ship with the skill.

Once the record exists, load the board doc it points to (`reference`) and proceed.

If the user is mid-conversation about a project and the board isn't confirmed yet, it's fine to discuss the project idea in general terms, but hold off on specific pin numbers, voltages, or compile commands until it is.

### Workshop demo sketches — use the reference code

The workshop teaches a fixed set of demo projects, and every student's code should look the same so instructors can help anyone at a glance. The reference code lives in [`references/workshop-sketches/`](references/workshop-sketches/):

| Sketch | What it does | Kit parts | Libraries | What the student changes |
|---|---|---|---|---|
| `scroll_name/` | Scrolls a name across the board's built-in red LED grid | Board only, no wiring | `ArduinoGraphics` + `Arduino_LED_Matrix` (built into the core) | The name in `matrix.println("  Philip  ")` |
| `oled_clock/` | Gets the time from the internet and shows date, time, and PST/PDT on the OLED | 0.96" OLED (SDA → A4, SCL → A5) | `WiFiS3`, `NTPClient`, `Adafruit SSD1306` | Their Wi-Fi name and password in `arduino_secrets.h` |

**Recognize them by intent, not exact wording.** "Show my name on the board", "scroll text on the LED grid", "make the lights spell my name" → `scroll_name`. "Build a clock", "show the time on the screen", "internet clock on the OLED" → `oled_clock`. If it's unclear whether they mean a demo or something different, ask once.

When a request matches a demo:

1. **Copy the reference sketch folder into the project directory as-is** (`./<sketch-name>/`, keeping the folder and file names, and every file in it). Don't write your own version, restructure it, rename variables, or "improve" it — matching code is the whole point.
2. **Change only the personalization points in the table**, nothing else:
   - `scroll_name`: ask the student what name (or short text) to scroll and put it in place of `Philip`, keeping the two spaces of padding on each side.
   - `oled_clock`: ask for their Wi-Fi network name and password and write them into `arduino_secrets.h` between the quotes. Remind them the board only joins **2.4 GHz** networks (many "5G" networks won't work). Don't repeat the password back in chat.
3. Still do every normal step around it: Step 3 (Wi-Fi firmware — essential before `oled_clock`), Step 5 (show the OLED wiring before they connect anything), and Step 4 (compile and upload).
4. **Variations build on the reference.** If they want the demo plus something extra ("the clock, but also show the temperature", "scroll my name faster"), start from the reference sketch and add the smallest change on top, keeping its existing structure, names, and comments, so it still looks like everyone else's. Only write from scratch when the project is genuinely different from every demo.

If a compile fails because `ArduinoGraphics` or `NTPClient` is missing (a student who skipped Install mode), install it with `arduino-cli lib install` and retry.

### Step 2 — Clarify the exact module whenever one comes up

This is an **ongoing** behavior for the whole conversation, not a one-time upfront question.

**Workshop kit first.** Every workshop student has the ELEGOO UNO R4 WiFi Super Starter Kit, listed part by part in [`references/kit.md`](references/kit.md) with models, pinouts, voltages, and libraries. When a project needs a part, look there first:
- **Exactly one kit part fits** (e.g. "measure distance" → HC-SR04, "temperature and humidity" → DHT11, "unlock with a card" → RC522): use it without asking — just tell them which part from their kit you're using, so they can find it. Use kit.md's specs and wiring; no web lookup needed.
- **Several kit parts fit** (e.g. "a display" → OLED, 7-segment, or the board's LED grid; "a motor" → servo, stepper, or fan motor): ask them to pick, offering **only the kit's options** (with a short plain description of what each is good for), plus "something not from the kit".
- **Not in the kit**, or they say they're using their own part: fall through to the general flow below.

General flow (non-kit parts, or a non-workshop user with no kit): whenever the user describes a piece of functionality that implies a hardware module — a display, a sensor, a motor, a wireless connection, a button, anything that isn't already using the board's own onboard I/O — and they haven't already named the specific part, stop and ask before writing code or wiring instructions for it.

Example: user says "I want to display this number on a display." Don't assume which display. Ask something like:

> "What display do you have for this? (e.g. 16x2 character LCD, small OLED like an SSD1306, 7-segment display, or the board's built-in LED matrix if it has one)"

Use `AskUserQuestion` with a few common options for that category of module plus room for "Other" (paste a product link or describe it) when there's a natural short list; ask a plain open question when there isn't. The goal is always to end up with a specific, identifiable part — a category name alone ("a display") isn't enough to give correct pins, voltage, or wiring.

Once the exact module is identified, look up its datasheet/pinout/voltage (`WebSearch`/`WebFetch`) before it's wired in — this feeds directly into Step 5. If the same module was already clarified and looked up earlier in the conversation, don't ask again.

### Step 3 — Wi-Fi firmware check, once, before the first upload (UNO R4 WiFi only)

The UNO R4 WiFi has a second chip (an ESP32-S3) that handles Wi-Fi, and its firmware is separate from the student's sketch. Boards that ship with old Wi-Fi firmware fail at things like getting the time from the internet — a clock project failed at a previous workshop for exactly this reason. So the **first time** a project is about to be uploaded, if `.arduino-board.json` has `wifi_firmware: null` and the FQBN is `arduino:renesas_uno:unor4wifi`:

1. Tell them plainly: "Before your first upload, I'm going to check the board's Wi-Fi software is up to date — old versions break internet features. This will replace the demo animation, which your project was about to do anyway." (This is why it's here and not in Check mode — reading the version means uploading a small helper program.)
2. Get the current port (`arduino-cli board list`), then read the installed version (uploads a helper sketch; needs internet the first time to fetch its helper tools):
   ```bash
   arduino-fwuploader firmware get-version -b arduino:renesas_uno:unor4wifi -a <port> --format json
   ```
3. Get the newest available version: `arduino-fwuploader firmware list -b arduino:renesas_uno:unor4wifi --format json` — the entry with `"Latest": true` has the newest `firmware_version`. (This one only reads Arduino's online list; it doesn't touch the board.)
4. If the installed version is older, update it — explain it takes a minute or two and that they must not unplug the board meanwhile:
   ```bash
   arduino-fwuploader firmware flash -b arduino:renesas_uno:unor4wifi -a <port>
   ```
   Re-run `arduino-cli board list` afterwards (the port can change), and re-run `get-version` to confirm the update took.
5. Write the confirmed version into `wifi_firmware` in `.arduino-board.json`, so this never runs again for this board. Then continue to the student's upload in Step 4.

If any of this fails, explain plainly what happened and still go ahead with the student's project upload — a project that doesn't use Wi-Fi doesn't need the update. Note it so you retry before any Wi-Fi project.

### Step 4 — Compile / upload workflow

**Sketches live inside the current project directory — never elsewhere.** Before creating or editing a sketch, confirm the working directory with `pwd`, and put the sketch under it (e.g. `./<sketch-name>/<sketch-name>.ino`) rather than in a temp dir, scratchpad, home directory, or any path outside the project. Don't let a sketch end up scattered somewhere unrelated to where the user is actually working — every `arduino-cli` command below should take a `<sketch-dir>` that resolves inside the project directory. If a sketch already exists in the project (check before creating a new one), edit it in place instead of scaffolding a second copy elsewhere.

See [`references/cli-cheatsheet.md`](references/cli-cheatsheet.md) for the full command reference (sketches, libraries, monitor). Core loop, using the FQBN from `.arduino-board.json` and the port fresh from `arduino-cli board list` each time (never reuse an old one, it can change between sessions):

```bash
arduino-cli sketch new <name>                                   # scaffold a new sketch, inside the project dir
arduino-cli compile --fqbn <fqbn> <sketch-dir>                  # compile only
arduino-cli upload -p <port> --fqbn <fqbn> <sketch-dir>         # upload only
arduino-cli compile --upload -p <port> --fqbn <fqbn> <sketch-dir>  # both in one shot
arduino-cli monitor -p <port> -c baudrate=115200                # read Serial output
```

**Flash after every change, every time — don't just compile and stop.** Whenever the sketch is created or edited in response to a user prompt, the turn isn't done until it's been compiled *and uploaded* to the board (`compile --upload`, or `compile` then `upload`). Don't wait for a separate "flash it" request — that's the default expectation for every prompt that touches code, not an extra step. The only exception is when the board genuinely isn't available (see Step 1) — say so plainly rather than silently skipping the upload. (On the very first upload, run Step 3 first.)

If compile fails on a missing library:
```bash
arduino-cli lib search <keyword>
arduino-cli lib install "<Library Name>"
```

### Step 5 — Always show pinout / wiring info before touching a new component

Every time a project introduces a new sensor, actuator, or module (after it's been clarified per Step 2), before telling the user to connect a single wire:

1. Print the relevant pin table from the board's context doc (which pins are free, which are PWM/analog/I2C/etc., voltage) plus the module's own pinout from Step 2.
2. Print a plain-text wiring map, one line per wire, in the form `<component pin> → <board pin>` — e.g.:
   ```
   DHT11 VCC  → 5V
   DHT11 GND  → GND
   DHT11 DATA → D2  (no extra resistor — the kit's DHT11 module has its pull-up built in)
   ```
3. Call out any voltage mismatch explicitly (e.g. a 3.3V-only sensor on a 5V board needs a level shifter or the 3.3V rail, never the 5V pin).
4. If the user wants a visual (not just text) wiring diagram, generate one as an SVG and publish it via the `Artifact` tool rather than trying to draw it in chat — reference `artifact-diagramming` skill guidance for how to make it legible. Default to the plain-text table above when a quick visual isn't specifically asked for; it's more reliable across every surface Codex runs in.

Never wire anything before this step, even if the user seems experienced — assume they aren't.

### Step 6 — Safety rules (always in effect)

See [`references/wiring-safety.md`](references/wiring-safety.md) for the full list. The non-negotiables:

- Never connect anything while the board is powered unless the user explicitly knows what "hot-plugging" risk means for that specific part.
- Double-check polarity (VCC/GND) and voltage rating *before* power is applied, not after something smells hot.
- Respect per-pin and total current limits (see the board's context doc) — don't drive motors/LEDs directly off a GPIO pin without a driver/transistor/resistor as appropriate.
- If a board or component gets hot, smells burnt, or shows smoke: disconnect power immediately and stop — don't troubleshoot live.
