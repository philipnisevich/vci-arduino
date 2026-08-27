# vci-arduino

This repo currently houses the **arduino-hardware** Claude Code skill: a board-agnostic skill for Arduino/Arduino-compatible hardware projects, written for non-technical users — it never assumes board specs, pin voltages, or which module someone means; it detects, confirms, and asks.

> This README documents what the skill does and how it's organized, for anyone (human or Claude) editing it. Keep it in sync — update it whenever `.claude/skills/arduino-hardware/SKILL.md` or its reference files change, in the same commit.

## Modes

The skill has three modes, dispatched from `SKILL.md`'s "What to do when invoked" section based on how it's triggered:

| Mode | Trigger | Purpose |
|---|---|---|
| **Setup** | `/arduino-hardware setup` | One-time (or re-run-on-demand) board detection. Confirms the board is plugged in, runs `arduino-cli board list`/`board details`/`compile --show-properties`, and writes a context document for that board. |
| **Specify** | `/arduino-hardware specify <board name, product link, or context>` | Manual override when `arduino-cli`'s USB-ID auto-detection is wrong or ambiguous (e.g. a Pro Micro clone reporting itself as a Leonardo). User-supplied info replaces the guess; the resulting doc is marked "confirmed by user, not auto-detected." |
| **Project work** (default) | Any other Arduino-related conversation | Ongoing project help: confirms setup has run, clarifies the exact module/component whenever one is mentioned (not an upfront batch question), compile/upload workflow, wiring/pinout display before touching a new component, and safety rules. |

## Why detection needs two sources, not one

`arduino-cli` can reliably tell you a board's **FQBN, MCU, clock speed, build flags, toolchain, and upload protocol** (via `board details --json` and `compile --show-properties`) — that's real data pulled straight from the installed core, not guesswork, and it's what compile commands are built from.

It **cannot** tell you a board's **electrical specs** — logic voltage, per-pin current limits, or pin map (which pins are PWM/analog/I2C/etc). That has to come from the board's own docs (web search), because it's not encoded anywhere `arduino-cli` reads from.

So every board context document has two sections, sourced differently, and both are required before wiring anything:
1. **CLI-sourced build data** — from `arduino-cli`, verbatim.
2. **Electrical specs (web-sourced)** — from a targeted web lookup, cited.

## Why detection can be wrong

`arduino-cli board list` identifies boards by USB vendor/product ID. Some clone boards deliberately reuse another official board's ID so generic OS drivers recognize them — the most common case is **Pro Micro clones reporting as an Arduino Leonardo**. Auto-detection is a best guess, not a certainty. Specify mode exists specifically to let the user's own knowledge correct it, and to leave a permanent note in the context doc so a future session doesn't get fooled back to the same wrong guess.

## File layout

```
vci-arduino/
├── README.md                                  # this file
└── .claude/skills/arduino-hardware/
    ├── SKILL.md                               # the skill itself — all three modes' instructions
    └── references/
        ├── cli-cheatsheet.md                  # arduino-cli command reference (setup, board/core discovery, sketches, upload, monitor, libraries)
        ├── wiring-safety.md                   # beginner-safe wiring rules
        └── boards/
            └── <fqbn_with_underscores>.md      # one per board ever detected/specified — CLI-sourced + web-sourced sections
```

Board context docs are named after their FQBN with `:`/`/` replaced by `_`, e.g. `arduino:renesas_uno:unor4wifi` → `arduino_renesas_uno_unor4wifi.md`. This directory is a growing cache: every new board encountered (via Setup or Specify mode) gets written back here, so a board only needs to go through full detection/lookup once.

## Maintenance

- Keep this README in sync with `SKILL.md` — same commit, every time either changes.
- New board context docs written by the skill during actual use are expected to accumulate in `references/boards/` — that's the design, not something to clean up.
