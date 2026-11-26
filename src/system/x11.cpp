// inspiration from https://github.com/robiot/xclicker/blob/master/src/x11api.c
#include <thread>
#include <chrono>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <linux/input-event-codes.h>

Display* display = nullptr;
Window root = nullptr;

bool clicking = false;
int interval = 1;

void click() { // left button right now
    XTestFakeButtonEvent(display, Button1, True, CurrentTime);
    XTestFakeButtonEvent(display, Button1, False, CurrentTime);
    XFlush(display);
}

void hotkeyToggle() {

}

int main() {
    display = XOpenDisplay(NULL);
    if (!display) return 1;

    // some stuff

    XCloseDisplay(display);
    return 0;
}