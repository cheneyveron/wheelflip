#!/bin/sh
# Install build/WheelFlip.app to ~/.local/share/wheelflip and run it at login via a LaunchAgent.
set -e
cd "$(dirname "$0")"
[ -d build/WheelFlip.app ] || ./build.sh
DEST="$HOME/.local/share/wheelflip"
PLIST="$HOME/Library/LaunchAgents/local.wheelflip.plist"
mkdir -p "$DEST"
launchctl bootout "gui/$(id -u)/local.wheelflip" 2>/dev/null || true
rm -rf "$DEST/WheelFlip.app" && cp -R build/WheelFlip.app "$DEST/"
cat > "$PLIST" <<PL
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>Label</key><string>local.wheelflip</string>
  <key>ProgramArguments</key><array><string>$DEST/WheelFlip.app/Contents/MacOS/wheelflip</string></array>
  <key>RunAtLoad</key><true/>
  <key>KeepAlive</key><true/>
  <key>ProcessType</key><string>Interactive</string>
</dict>
</plist>
PL
launchctl bootstrap "gui/$(id -u)" "$PLIST"
echo "installed; grant WheelFlip in System Settings → Privacy & Security → Accessibility"
