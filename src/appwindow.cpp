// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#include "appwindow.hpp"
#include <iostream>
using namespace std;

AppWindow::AppWindow()
: game ( new Game(0) ),
  viewer(*game) {
    set_title("CS488 Project");

    // A utility class for constructing things that go into menus, which
    // we'll set up next.
    using Gtk::Menu_Helpers::MenuElem;
    using Gtk::Menu_Helpers::RadioMenuElem;
    using Gtk::Menu_Helpers::CheckMenuElem;
    using Gtk::Menu_Helpers::SeparatorElem;

    // Set up the Application menu
    // The slot we use here just causes AppWindow::hide() on this,
    // which shuts down the application.
    menuApp.items().push_back(
            MenuElem("_Quit", Gtk::AccelKey("q"),
                    sigc::mem_fun(*this, &AppWindow::hide)));


    // sky box
    menuOptions.items().push_back( CheckMenuElem("Enable _Sky Box", Gtk::AccelKey("s")) );
    Gtk::CheckMenuItem* enSkyBox = static_cast<Gtk::CheckMenuItem*>(&menuOptions.items().back());
    enSkyBox->set_active();
    enSkyBox->signal_toggled().connect(sigc::mem_fun( viewer, &Viewer::toggleSkyBox ));

    menuOptions.items().push_back( SeparatorElem() );

    // glow
    sigc::slot1<void, int> set_glow_slot = sigc::mem_fun(viewer, &Viewer::setGlow);
    Gtk::RadioButtonGroup set_glow_group;
    menuOptions.items().push_back( RadioMenuElem(set_glow_group, "_No Glow", Gtk::AccelKey("F1"),
      sigc::bind( set_glow_slot, 0 ) ) );
    menuOptions.items().push_back( RadioMenuElem(set_glow_group, "_Small Glow", Gtk::AccelKey("F2"),
      sigc::bind( set_glow_slot, 1 ) ) );
    menuOptions.items().push_back( RadioMenuElem(set_glow_group, "_Big Glow", Gtk::AccelKey("F3"),
      sigc::bind( set_glow_slot, 2 ) ) );
    Gtk::RadioMenuItem* bigGlow = static_cast<Gtk::RadioMenuItem*>(&menuOptions.items().back());
    bigGlow->set_active();

    menuOptions.items().push_back( SeparatorElem() );

    // particle system
    menuOptions.items().push_back( CheckMenuElem("Enable _Particle System", Gtk::AccelKey("p")) );
    Gtk::CheckMenuItem* enPartSys = static_cast<Gtk::CheckMenuItem*>(&menuOptions.items().back());
    enPartSys->set_active();
    enPartSys->signal_toggled().connect(sigc::mem_fun( viewer, &Viewer::toggleParticleSystem ));

    // level
    sigc::slot1<void, int> set_level_slot = sigc::mem_fun(this, &AppWindow::setLevel);
    Gtk::RadioButtonGroup set_level_group;
    menuLevel.items().push_back( RadioMenuElem(set_level_group, "Level _1", Gtk::AccelKey("1"),
      sigc::bind( set_level_slot, 0 ) ) );
    menuLevel.items().push_back( RadioMenuElem(set_level_group, "Level _2", Gtk::AccelKey("2"),
      sigc::bind( set_level_slot, 1 ) ) );
    menuLevel.items().push_back( RadioMenuElem(set_level_group, "Level _3", Gtk::AccelKey("3"),
      sigc::bind( set_level_slot, 2 ) ) );

    menuAbout.items().push_back(
            MenuElem("_Credits", Gtk::AccelKey("c"),
                    sigc::mem_fun(*this, &AppWindow::credits)));
    menuAbout.items().push_back(
            MenuElem("_About", Gtk::AccelKey("a"),
                    sigc::mem_fun(*this, &AppWindow::about)));

    // Set up the menu bar
    menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", menuApp));
    menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Level", menuLevel));
    menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Options", menuOptions));
    menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Info", menuAbout));

    // First add the vertical box as our single "top" widget
    add(vbox);

    // Put the menubar on the top, and make it as small as possible
    vbox.pack_start(menubar, Gtk::PACK_SHRINK);

    set_default_size(768, 768);
    vbox.pack_start(viewer);

    show_all();

    timerConnection = Glib::signal_timeout().connect(
            sigc::mem_fun(this, &AppWindow::update), 20);
}

void AppWindow::setLevel(int l) {
    delete game;
    game = new Game(l);
    viewer.setGame(*game);
}

bool AppWindow::update() {
    game->update(0.02);
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
        game->setKey(MoveForward, pressed);
        break;
    case GDK_KEY_Down:
        game->setKey(MoveBackward, pressed);
        break;
     case GDK_KEY_Left:
         game->setKey(MoveLeft, pressed);
         break;
     case GDK_KEY_Right:
         game->setKey(MoveRight, pressed);
         break;
     /*case ' ':
     m_game->drop(); break;*/
     default:
         if (pressed)
             return Gtk::Window::on_key_press_event( ev );
         else
             return false;
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

void AppWindow::about() {
    Gtk::MessageDialog dialog(*this, "About CS 488 game project",
            false /* use_markup */, Gtk::MESSAGE_QUESTION,
            Gtk::BUTTONS_OK);
    dialog.set_secondary_text(
            "Author: Christian Zommerfelds\n"
            "Copyright (c) 2012");

    dialog.run();
}

void AppWindow::credits() {
    Gtk::MessageDialog dialog(*this, "Credits",
            false /* use_markup */, Gtk::MESSAGE_QUESTION,
            Gtk::BUTTONS_OK);
    dialog.set_secondary_text(
            "Sky box by Hazel Whorley\n"
            "http://gamebanana.com/game/textures/327\n\n"
            "Wheel texture by NetAlloy\n"
            "http://www.clker.com/clipart-wheel-2.html\n\n"
            "All other textures by Spiral Graphics Inc.\n"
            "http://www.spiralgraphics.biz/");

    dialog.run();
}
