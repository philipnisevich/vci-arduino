# arduino-cli command reference

## Setup (one-time per machine)
```bash
bash .claude/skills/arduino-hardware/scripts/install-tools.sh               # tools + UNO R4 WiFi core
bash .claude/skills/arduino-hardware/scripts/install-tools.sh --tools-only  # tools only
```
Installs `arduino-cli` and `arduino-fwuploader` into `~/.local/bin` from Arduino's official downloads — no Homebrew, no `sudo`, no password — then runs `arduino-cli core update-index`. Safe to re-run. If a tool isn't on `PATH`, call it as `~/.local/bin/arduino-cli` / `~/.local/bin/arduino-fwuploader`.

## Discover / install a board's core
```bash
arduino-cli board list                        # what's plugged in right now, with port + FQBN if known
arduino-cli board search <text>                # look up a board by name, get its FQBN/platform id
arduino-cli board details -b <fqbn>            # package info, programmer options, USB VID/PID
arduino-cli core list                          # cores already installed on this machine
arduino-cli core install <platform id>         # e.g. arduino-cli core install arduino:renesas_uno
```

`board list` shows "Unknown" for a connected board when its core isn't installed yet — that's expected, not an error. Install the core, then re-run `board list`.

## Sketches
```bash
arduino-cli sketch new <name>                              # scaffolds <name>/<name>.ino
arduino-cli compile --fqbn <fqbn> <sketch-dir>              # compile, no upload
arduino-cli upload -p <port> --fqbn <fqbn> <sketch-dir>     # upload a previously compiled build
arduino-cli compile --upload -p <port> --fqbn <fqbn> <sketch-dir>   # compile + upload together
```

The sketch's `.ino` file must have the same name as its containing folder — `arduino-cli sketch new` handles this automatically; don't rename one without the other.

## Serial monitor
```bash
arduino-cli monitor -p <port> -c baudrate=115200
```
Baud rate must match whatever the sketch sets via `Serial.begin(...)`. Ctrl+C to exit.

## Libraries
```bash
arduino-cli lib search <keyword>
arduino-cli lib install "<Library Name>"
arduino-cli lib list                            # what's installed
arduino-cli lib upgrade                         # upgrade all installed libraries
```

## Useful flags
- `--json` on most commands (e.g. `arduino-cli board list --json`) for reliable machine-parseable output instead of scraping table text.
- `-v` for verbose compiler/upload output when diagnosing a failure.
- `--warnings all` on `compile` to surface compiler warnings that hide real bugs.

## Finding the port
The port (e.g. `/dev/cu.usbmodemXXXX` on macOS, `/dev/ttyACM0`/`/dev/ttyUSB0` on Linux, `COM3` on Windows) can change across reboots or when switching USB ports/cables. Always re-run `arduino-cli board list` right before upload rather than reusing a port from earlier in the conversation.

## Wi-Fi module firmware (UNO R4 WiFi's ESP32-S3)
```bash
arduino-fwuploader firmware list -b arduino:renesas_uno:unor4wifi --format json                  # versions available
arduino-fwuploader firmware get-version -b arduino:renesas_uno:unor4wifi -a <port> --format json  # what's on the board
arduino-fwuploader firmware flash -b arduino:renesas_uno:unor4wifi -a <port>                      # update to latest
```
`get-version` and `flash` both upload a helper sketch to the board, replacing whatever was running (including the factory demo animation). Never run them during `/arduino-hardware check`.
