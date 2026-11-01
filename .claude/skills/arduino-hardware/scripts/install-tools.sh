#!/usr/bin/env bash
# Installs everything the arduino-hardware skill needs, without a board and
# without a password: arduino-cli, arduino-fwuploader, and the board core.
#
#   bash install-tools.sh               # tools + the workshop board's core (arduino:renesas_uno)
#   bash install-tools.sh --tools-only  # tools only (setup mode installs the right core itself)
#
# Never touches a connected board. Never uses sudo or Homebrew: binaries are
# downloaded from Arduino's official release builds into ~/.local/bin, a
# folder the user owns. Safe to re-run; anything already installed is kept.
set -euo pipefail

WORKSHOP_CORE="arduino:renesas_uno"
FWUPLOADER_VERSION="2.4.1"
BIN_DIR="$HOME/.local/bin"
# Libraries for the parts in references/kit.md (dependencies install automatically).
KIT_LIBRARIES=(
  "Adafruit SSD1306"    # 0.96" OLED
  "DHT sensor library"  # DHT11
  "MFRC522"             # RC522 RFID reader
  "IRremote"            # IR receiver / emitter
  "Keypad"              # 4x4 membrane keypad
  "MPU9250_WE"          # GY-6500 (MPU-6500)
  "Servo"               # SG90
  "Stepper"             # 28BYJ-48 + ULN2003
  "AccelStepper"        # 28BYJ-48, smooth motion
)

INSTALL_CORE=1
if [ "${1:-}" = "--tools-only" ]; then INSTALL_CORE=0; fi

say() { printf '==> %s\n' "$*"; }
fail() { printf 'ERROR: %s\n' "$*" >&2; exit 1; }

case "$(uname -s)" in
  Darwin) OS=macOS; EXT=tar.gz; EXE="" ;;
  Linux) OS=Linux; EXT=tar.gz; EXE="" ;;
  MINGW* | MSYS* | CYGWIN*) OS=Windows; EXT=zip; EXE=".exe" ;;
  *) fail "Unsupported operating system: $(uname -s)" ;;
esac
case "$(uname -m)" in
  arm64 | aarch64) ARCH=ARM64 ;;
  x86_64 | amd64) ARCH=64bit ;;
  *) fail "Unsupported processor: $(uname -m)" ;;
esac
# Arduino doesn't publish Windows ARM builds; the 64bit one runs under emulation.
if [ "$OS" = Windows ]; then ARCH=64bit; fi

mkdir -p "$BIN_DIR"

# Prints the path of an already-installed tool, or nothing.
find_tool() {
  if command -v "$1" >/dev/null 2>&1; then
    command -v "$1"
  elif [ -x "$BIN_DIR/$1$EXE" ]; then
    echo "$BIN_DIR/$1$EXE"
  fi
}

# download_tool <name> <url>: fetches an official release archive and puts
# just the binary into $BIN_DIR.
download_tool() {
  local name=$1 url=$2 tmp
  tmp=$(mktemp -d)
  say "Downloading $name from $url"
  curl -fsSL -o "$tmp/pkg.$EXT" "$url" || fail "Couldn't download $name — check the internet connection and try again."
  if [ "$OS" = Windows ]; then
    powershell.exe -NoProfile -Command \
      "Expand-Archive -Force -Path '$(cygpath -w "$tmp/pkg.$EXT")' -DestinationPath '$(cygpath -w "$tmp/out")'"
  else
    mkdir "$tmp/out"
    tar -xzf "$tmp/pkg.$EXT" -C "$tmp/out"
  fi
  cp "$tmp/out/$name$EXE" "$BIN_DIR/$name$EXE"
  chmod +x "$BIN_DIR/$name$EXE"
  rm -rf "$tmp"
}

ACLI=$(find_tool arduino-cli)
if [ -n "$ACLI" ]; then
  say "arduino-cli already installed at $ACLI — keeping it"
else
  download_tool arduino-cli "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_${OS}_${ARCH}.${EXT}"
  ACLI="$BIN_DIR/arduino-cli$EXE"
fi
"$ACLI" version >/dev/null || fail "arduino-cli was installed but won't run."

FWU=$(find_tool arduino-fwuploader)
if [ -n "$FWU" ]; then
  say "arduino-fwuploader already installed at $FWU — keeping it"
else
  download_tool arduino-fwuploader \
    "https://downloads.arduino.cc/arduino-fwuploader/arduino-fwuploader_${FWUPLOADER_VERSION}_${OS}_${ARCH}.${EXT}"
  FWU="$BIN_DIR/arduino-fwuploader$EXE"
fi
"$FWU" version >/dev/null || fail "arduino-fwuploader was installed but won't run."

say "Refreshing the list of boards"
"$ACLI" core update-index >/dev/null

if [ "$INSTALL_CORE" = 1 ]; then
  say "Installing board support for $WORKSHOP_CORE (this is the big download — a few minutes is normal)"
  "$ACLI" core install "$WORKSHOP_CORE"

  # The UNO R4 core's compiler (arm-none-eabi-gcc 7-2017q4) only ships as an
  # Intel build for macOS, so Apple Silicon Macs need Rosetta 2 to compile.
  if [ "$OS" = macOS ] && [ "$ARCH" = ARM64 ] && ! arch -x86_64 /usr/bin/true 2>/dev/null; then
    say "Installing Rosetta (Apple's helper that lets this Mac run the board's compiler)"
    softwareupdate --install-rosetta --agree-to-license ||
      fail "Couldn't install Rosetta. The board's compiler needs it on this Mac."
  fi

  say "Installing code libraries for the parts in the workshop kit"
  "$ACLI" lib install "${KIT_LIBRARIES[@]}"
fi

say "Done."
echo "ARDUINO_CLI=$ACLI"
echo "ARDUINO_FWUPLOADER=$FWU"
