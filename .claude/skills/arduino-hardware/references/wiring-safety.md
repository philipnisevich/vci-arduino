# Wiring safety rules

Written for someone who has never wired electronics before. Apply these every time, regardless of how simple the component seems.

## Before connecting anything
1. **Power off / unplugged first.** Make all wiring changes with the board disconnected from USB and any external power (barrel jack/battery). Only reconnect power once wiring is complete and double-checked.
2. **Check voltage compatibility.** Look up the component's operating voltage (its datasheet or listing) and compare against the board pin you're using (see the board's reference file, e.g. 5V vs 3.3V logic). Mismatches are the #1 cause of fried components.
3. **Check polarity.** VCC/+ and GND/− must go to the right pins. Reversed power on many modules destroys them instantly. Most breakout boards mark this silkscreen-style on the PCB — read it before connecting.
4. **Check current draw.** Small sensors are fine straight off a GPIO pin. Anything that moves (motor, fan, solenoid) or is bright (high-power LED strip) needs a transistor/MOSFET/driver board or relay module — never wire it directly to a signal pin.

## While wiring
- Connect ground (GND) first, signal/data pins last, if a component supports it — reduces the chance of a floating/undefined state damaging something.
- Use a breadboard for anything experimental before soldering — mistakes should be a matter of moving a wire, not desoldering.
- Keep exposed wires away from each other — a stray short between VCC and GND, or between two signal pins, can be enough to damage the board or component.

## After wiring, before powering on
- Visually re-trace every wire against the wiring map you printed in Project work Step 5 of the main skill.
- If anything looks uncertain, don't guess — ask, or look up the exact component's datasheet.

## While powered
- If you smell something burning, see smoke, or a component is hot to the touch: **disconnect power immediately.** Don't try to "just check one thing" first.
- Don't add or remove wires while the circuit is powered ("hot-plugging") unless you specifically know the component supports it (e.g. USB, some I2C Qwiic/STEMMA QT connectors are designed for it — most jumper-wire breadboard connections are not).
- Don't leave a project powered and unattended for long stretches while still in the debugging/experimental wiring phase.

## When something doesn't work
Symptoms like "nothing happens," "board resets randomly," or "gets warm" are usually wiring/power issues, not code issues — re-check wiring against the board's current limits and voltage before assuming the sketch is wrong.
