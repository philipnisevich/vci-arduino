# vci-arduino

A Claude Code skill that helps you build Arduino projects — even if you've never done electronics before. It figures out what board you have, tells you exactly how to wire things up, and keeps you from frying anything.

## How to use it

**First time with a board:**
```
/arduino-hardware setup
```
Plug your board in via USB and run this. Claude will detect it and remember it for next time.

**Building a project:**
Just describe what you want to build (e.g. "I want to show a number on a display"). Claude will ask what specific part you're using, then walk you through the wiring and code.

If your board gets detected wrong (this happens with some clone boards), tell Claude what board it actually is, or run:
```
/arduino-hardware setup <board name or product link>
```

## Safety

Claude will always show you the wiring before you connect anything, and will warn you about voltage mismatches or current limits. The short version:
- Don't wire anything while the board is powered.
- Double-check + and − before powering on.
- If anything smells hot or smokes, unplug it immediately.
