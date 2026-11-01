# vci-arduino

A Claude Code skill that helps you build Arduino projects — even if you've never done electronics before. It figures out what board you have, tells you exactly how to wire things up, and keeps you from frying anything. You never need to open a terminal or type commands — Claude does all of that.

## How to use it

**Before the workshop (at home, kit still closed):**
```
/arduino-hardware install
```
Claude downloads the free Arduino tools and support for your board. It takes about 10 minutes, doesn't need the board, and won't ask for your password. Keep your computer online and awake while it runs.

**At the workshop, when you first plug in your board:**
```
/arduino-hardware check
```
Plug the board in with the kit's cable — the grid of red lights should play an animation. (The cable's computer end is the older, wider USB-A plug; if your laptop only has USB-C ports, bring an adapter.) Claude checks the board is connected properly, without changing anything on it. The animation keeps playing until you build your first project.

**Building a project:**
Just describe what you want to build (e.g. "I want to show a number on a display"). Claude already knows every part in the workshop kit, so it'll tell you which one to grab — and only ask when the kit has more than one that would work. Then it walks you through the wiring and code. Before your very first upload, Claude also makes sure the board's Wi-Fi software is up to date, so internet features (like getting the time) work.

**Using a different board?** Run:
```
/arduino-hardware setup
```
If your board gets detected wrong (this happens with some clone boards), tell Claude what board it actually is, or run:
```
/arduino-hardware setup <board name or product link>
```

## Safety

Claude will always show you the wiring before you connect anything, and will warn you about voltage mismatches or current limits. The short version:
- Don't wire anything while the board is powered.
- Double-check + and − before powering on.
- If anything smells hot or smokes, unplug it immediately.
