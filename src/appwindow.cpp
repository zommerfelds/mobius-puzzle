// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#include "appwindow.hpp"
#include <iostream>

AppWindow::AppWindow()
: m_viewer ()
{
  set_title("CS488 Project");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  using Gtk::Menu_Helpers::RadioMenuElem;
  using Gtk::Menu_Helpers::CheckMenuElem;
  
  // Set up the Application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));
  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(m_vbox);

  // Put the menubar on the top, and make it as small as possible
  m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

  set_default_size(500, 500);
  m_vbox.pack_start(m_viewer);

  show_all();
  
  m_timerConnection = Glib::signal_timeout().connect(sigc::mem_fun( this, &AppWindow::update ), 20);
}

bool AppWindow::update()
{
  //m_game.tick();
  //m_viewer.invalidate();
  return true; // do not disconnect handler
}

bool AppWindow::on_key_press_event( GdkEventKey *ev )
{
  // This is a convenient place to handle non-shortcut
  // keys.  You'll want to look at ev->keyval.

	// An example key; delete and replace with the
	// keys you want to process
  /*switch( ev->keyval ) {
  case GDK_KEY_Up:
    m_game.rotateCCW(); break;
  case GDK_KEY_Down:
    m_game.rotateCW(); break;
  case GDK_KEY_Left:
    m_game.moveLeft(); break;
  case GDK_KEY_Right:
    m_game.moveRight(); break;
  case ' ':
    m_game.drop(); break;
  default:
    return Gtk::Window::on_key_press_event( ev );
  }*/
  m_viewer.invalidate();
  return true;
}
