#include <libportal/globalshortcuts.h>
#include <libportal/portal.h>
#include <libportal/remote.h>

#include <linux/input-event-codes.h>
#include <glib.h>

GMainLoop* loop = nullptr; // handles the main thread
XdpPortal* portal = nullptr;
XdpSession* session = nullptr;
XdpGlobalShortcutsSession* shortcutSession = nullptr;

GPtrArray* shortcuts = nullptr;
gboolean clicking = false;
guint timer = 0; // must use glib's types

int interval = 1;

// https://libportal.org/index.html

void click() {
    xdp_session_pointer_button(session, BTN_LEFT, XDP_BUTTON_PRESSED);
    xdp_session_pointer_button(session, BTN_LEFT, XDP_BUTTON_RELEASED);
}

gboolean clickingTimer(gpointer data) {
    if (clicking) click();
    return G_SOURCE_CONTINUE; // equivalent of true
}

// https://libportal.org/signal.GlobalShortcutsSession.activated.html
void hotkeyToggle(XdpGlobalShortcutsSession* source, gchar* id, guint64 timestamp, GVariant* options, gpointer data) {
    if (strcmp(id, "toggle") == 0) { // checks if string given equals toggle
        clicking = !clicking;
    }
}

/*
https://stackoverflow.com/questions/73939528
https://docs.gtk.org/gio/callback.AsyncReadyCallback.html

Type: GObject | Name: source | Function: current portal/session object
Type: GAsyncResult | Name: result | Function: Contains the resulting yield of an async function
Type: gpointer | Name: data | Function: Custom data to pass
*/

void call(GObject* source, GAsyncResult* result, gpointer data) {
    if (!xdp_session_start_finish(session, result, nullptr)) { // if session fails to start, exit
        g_main_loop_quit(loop);
        return;
    }

    // async, no blocking
    timer = g_timeout_add(interval, clickingTimer, nullptr);
}

void initMouse(GObject* source, GAsyncResult* result, gpointer data) {
    session = xdp_portal_create_remote_desktop_session_finish(portal, result, nullptr); // creates the session
    if (!session) {
        g_main_loop_quit(loop);
        return;
    }

    xdp_session_start(session, nullptr, nullptr, call, nullptr);
}

void initKeyboard(GObject* source, GAsyncResult* result, gpointer data) {
    shortcutSession = xdp_portal_create_global_shortcuts_session_finish(portal, result, nullptr); // creates the shortcut listener
    if (!shortcutSession) {
        g_main_loop_quit(loop);
        return;
    }

    g_signal_connect(shortcutSession, "activated", G_CALLBACK(hotkeyToggle), nullptr);

    shortcuts = g_ptr_array_new_with_free_func ((GDestroyNotify) xdp_global_shortcut_free); // array
    g_ptr_array_add(shortcuts, xdp_global_shortcut_new("toggle", "Toggle autoclicker", "F6"));

    xdp_global_shortcuts_session_bind_shortcuts(shortcutSession, shortcuts, nullptr, nullptr, nullptr, nullptr);
}

int main() {
    loop = g_main_loop_new(nullptr, false);
    portal = xdp_portal_new();

    xdp_portal_create_remote_desktop_session(
        portal,
        XDP_DEVICE_POINTER,                                     // asks for pointer access
        XDP_OUTPUT_NONE,                                        // don't ask to select which monitor/window
        XDP_REMOTE_DESKTOP_FLAG_NONE,          // use current session options
        XDP_CURSOR_MODE_HIDDEN,                       // hides the created cursor (distracting sometimes)
        nullptr, initMouse, nullptr                              // calls the init function
    );

    xdp_portal_create_global_shortcuts_session(portal, nullptr, initKeyboard, nullptr);

    g_main_loop_run(loop);

    if (session) { // unbindings
        xdp_session_close(session);
        g_object_unref(session);
    }

    if (shortcutSession) {
        xdp_global_shortcuts_session_close(shortcutSession);
        g_object_unref(shortcutSession);
    }

    if (shortcuts) g_ptr_array_unref(shortcuts);

    g_object_unref(portal); // unbindings
    g_main_loop_unref(loop);

    return 0;
}

/*
Basic structure goes:
- Create a main thread
- Create portal which requests a mouse session
- Then requests a keyboard session
- Sessions listen for keybind and does clicking
- Exits, ending thread, clearing
*/