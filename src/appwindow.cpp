// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#include "appwindow.hpp"
#include <iostream>
using namespace std;

AppWindow::AppWindow()
: game (),
  viewer(game) {
    set_title("CS488 Project");

    // A utility class for constructing things that go into menus, which
    // we'll set up next.
    using Gtk::Menu_Helpers::MenuElem;
    using Gtk::Menu_Helpers::RadioMenuElem;
    using Gtk::Menu_Helpers::CheckMenuElem;

    // Set up the Application menu
    // The slot we use here just causes AppWindow::hide() on this,
    // which shuts down the application.
    m_menu_app.items().push_back(
            MenuElem("_Quit", Gtk::AccelKey("q"),
                    sigc::mem_fun(*this, &AppWindow::hide)));
    // Pack in our widgets

    // First add the vertical box as our single "top" widget
    add(m_vbox);

    // Put the menubar on the top, and make it as small as possible
    m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

    set_default_size(500, 500);
    m_vbox.pack_start(viewer);

    show_all();

    m_timerConnection = Glib::signal_timeout().connect(
            sigc::mem_fun(this, &AppWindow::update), 20);
}

bool AppWindow::update() {
    game.update(0.02);
    viewer.invalidate();
    return true;// do not disconnect handler
}

bool AppWindow::handleKey(GdkEventKey* ev) {
    // This is a convenient place to handle non-shortcut
    // keys.  You'll want to look at ev->keyval.

    bool pressed = (ev->type == GDK_KEY_PRESS);

    // An example key; delete and replace with the
    // keys you want to process
    switch( ev->keyval ) {
    case GDK_KEY_Up:
        game.setKey(MoveForward, pressed);
        break;
    case GDK_KEY_Down:
        game.setKey(MoveBackward, pressed);
        break;
     case GDK_KEY_Left:
         game.setKey(MoveLeft, pressed);
         break;
     case GDK_KEY_Right:
         game.setKey(MoveRight, pressed);
         break;
     /*case ' ':
     m_game.drop(); break;*/
     default:
         return Gtk::Window::on_key_press_event( ev );
     }
    //m_viewer.invalidate();
    return true;
}

bool AppWindow::on_key_release_event(GdkEventKey *ev) {
    return handleKey(ev);
}

bool AppWindow::on_key_press_event(GdkEventKey *ev) {
    return handleKey(ev);
}
