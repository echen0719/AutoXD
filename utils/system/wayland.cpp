#include <libportal/portal.h>
#include <libportal/remote.h>

#include <linux/input-event-codes.h>
#include <glib.h>

GMainLoop* loop = nullptr; // handles the main thread
XdpPortal* portal = nullptr;
XdpSession* session = nullptr;

// https://libportal.org/index.html

void click() {
    xdp_session_pointer_button(session, BTN_LEFT, XDP_BUTTON_PRESSED);
    xdp_session_pointer_button(session, BTN_LEFT, XDP_BUTTON_RELEASED);
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
    g_timeout_add(1, [](gpointer) -> gboolean {
        click();
        return G_SOURCE_CONTINUE;
    }, nullptr);
}

void init(GObject* source, GAsyncResult* result, gpointer data) {
    session = xdp_portal_create_remote_desktop_session_finish(portal, result, nullptr); // creates the session
    if (!session) {
        g_main_loop_quit(loop);
        return;
    }

    xdp_session_start(session, nullptr, nullptr, call, nullptr);
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
        nullptr, init, nullptr                                          // calls our init function
    );

    g_main_loop_run(loop);

    if (session) { // unbindings
        xdp_session_close(session);
        g_object_unref(session);
    }

    g_object_unref(portal); // unbindings
    g_main_loop_unref(loop);

    return 0;
}

/*
Basic structure goes:
- Create a main thread
- Create portal which requests a session
- Session does clicking
- Exits, ending thread
*/