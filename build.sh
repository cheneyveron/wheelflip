#!/bin/sh
# Build build/WheelFlip.app. Signs with $SIGN_IDENTITY (a codesign identity name or hash);
# without it the app is ad-hoc signed, which works but loses its Accessibility grant on every rebuild.
set -e
cd "$(dirname "$0")"
APP=build/WheelFlip.app
rm -rf "$APP" && mkdir -p "$APP/Contents/MacOS"
cp Info.plist "$APP/Contents/Info.plist"
clang -O2 -Wall -Wextra -arch arm64 -arch x86_64 -o "$APP/Contents/MacOS/wheelflip" wheelflip.c -framework ApplicationServices
codesign -f -s "${SIGN_IDENTITY:--}" --identifier local.wheelflip "$APP"
codesign -dv "$APP" 2>&1 | grep -E 'Signature|Authority' | head -2
echo "built $APP"
