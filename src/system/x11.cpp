// inspiration from https://github.com/robiot/xclicker/blob/master/src/x11api.c
#include <iostream>
#include <thread>
#include <chrono>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

Display* display = nullptr;
Window root = 0;
KeyCode toggleKey = 0; // int

bool clicking = false;
int interval = 1;

void click() { // left button right now
    XTestFakeButtonEvent(display, Button1, True, CurrentTime);
    XTestFakeButtonEvent(display, Button1, False, CurrentTime);
    XFlush(display);
}

void hotkeyToggle() {
    clicking = !clicking;
}

void initKeyboard () { // register key with Xorg server
    toggleKey = XKeysymToKeycode(display, XK_F6);

    // defines to pick up hotkey from anywhere on async
    XGrabKey(display, toggleKey, 0, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
}

void initDisplay() {
    display = XOpenDisplay(nullptr);
}

void call() {
    auto lastClick = std::chrono::steady_clock::now();

    while (true) {
        if (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event); // get event data, checking if it is toggle

            if (event.type == KeyPress && event.xkey.keycode == toggleKey) {
                hotkeyToggle();
            }
        }

        auto now = std::chrono::steady_clock::now();

        if (clicking && now - lastClick >= std::chrono::milliseconds(interval)) {
            click();
            lastClick = now;
            std::this_thread::sleep_for(std::chrono::microseconds(100)); // so to not spam CPU
        }

        if (!clicking) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // so to not spam CPU when toggled off
        }
    }
}

int main() {
    initDisplay();
    if (!display) {
        std::cout << "Failed to open display" << std::endl;
        return 1;
    }

    root = DefaultRootWindow(display);
    if (!root) return 1;

    initKeyboard();
    if (!toggleKey) return 1;

    call();

    XCloseDisplay(display);
    return 0;
}