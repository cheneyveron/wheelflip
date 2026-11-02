// wheelflip: reverse scroll direction for discrete mouse wheels only.
// Trackpads / Magic Mouse send continuous scroll events and pass through untouched.
// Only scroll-wheel events are tapped; pointer movement never goes through here.
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

static CFMachPortRef tap;

static CGEventRef cb(CGEventTapProxy p, CGEventType type, CGEventRef e, void *ctx) {
    (void)p; (void)ctx;
    if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
        CGEventTapEnable(tap, true);
        return e;
    }
    if (CGEventGetIntegerValueField(e, kCGScrollWheelEventIsContinuous) != 0)
        return e;
    // Read everything first: setting DeltaAxis makes CoreGraphics recompute the
    // FixedPt/Point fields, so flipping them one by one would undo the change.
    int64_t d1 = CGEventGetIntegerValueField(e, kCGScrollWheelEventDeltaAxis1);
    int64_t d2 = CGEventGetIntegerValueField(e, kCGScrollWheelEventDeltaAxis2);
    double  f1 = CGEventGetDoubleValueField(e, kCGScrollWheelEventFixedPtDeltaAxis1);
    double  f2 = CGEventGetDoubleValueField(e, kCGScrollWheelEventFixedPtDeltaAxis2);
    int64_t p1 = CGEventGetIntegerValueField(e, kCGScrollWheelEventPointDeltaAxis1);
    int64_t p2 = CGEventGetIntegerValueField(e, kCGScrollWheelEventPointDeltaAxis2);
    CGEventSetIntegerValueField(e, kCGScrollWheelEventDeltaAxis1, -d1);
    CGEventSetIntegerValueField(e, kCGScrollWheelEventDeltaAxis2, -d2);
    CGEventSetDoubleValueField(e, kCGScrollWheelEventFixedPtDeltaAxis1, -f1);
    CGEventSetDoubleValueField(e, kCGScrollWheelEventFixedPtDeltaAxis2, -f2);
    CGEventSetIntegerValueField(e, kCGScrollWheelEventPointDeltaAxis1, -p1);
    CGEventSetIntegerValueField(e, kCGScrollWheelEventPointDeltaAxis2, -p2);
    return e;
}

int main(void) {
    // A running process never sees a newly granted Accessibility permission,
    // so without it exit and let launchd (KeepAlive) start a fresh one.
    // Prompt only once so the relaunch loop doesn't keep popping dialogs;
    // the prompt also adds us to the Accessibility list.
    if (!AXIsProcessTrusted()) {
        char marker[1024];
        snprintf(marker, sizeof marker, "%s/.local/share/wheelflip/.prompted", getenv("HOME"));
        if (access(marker, F_OK) != 0) {
            const void *k[] = { kAXTrustedCheckOptionPrompt }, *v[] = { kCFBooleanTrue };
            CFDictionaryRef o = CFDictionaryCreate(NULL, k, v, 1, NULL, NULL);
            AXIsProcessTrustedWithOptions(o);
            close(open(marker, O_CREAT | O_WRONLY, 0644));
        }
        sleep(5);
        return 1;
    }
    tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
                           CGEventMaskBit(kCGEventScrollWheel), cb, NULL);
    if (!tap) return 1;
    CFRunLoopAddSource(CFRunLoopGetCurrent(),
                       CFMachPortCreateRunLoopSource(NULL, tap, 0), kCFRunLoopCommonModes);
    CGEventTapEnable(tap, true);
    CFRunLoopRun();
    return 0;
}
