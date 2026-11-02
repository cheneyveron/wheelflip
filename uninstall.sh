#!/bin/sh
launchctl bootout "gui/$(id -u)/local.wheelflip" 2>/dev/null || true
rm -f "$HOME/Library/LaunchAgents/local.wheelflip.plist"
rm -rf "$HOME/.local/share/wheelflip"
tccutil reset Accessibility local.wheelflip 2>/dev/null || true
echo "uninstalled"
