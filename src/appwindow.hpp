// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <gtkmm.h>
#include "viewer.hpp"

class AppWindow : public Gtk::Window {
public:
  AppWindow();
  
protected:
  virtual bool on_key_press_event( GdkEventKey *ev );

private:
  
  enum Speed { SLOW=0, MEDIUM, FAST };

  bool update();
  void setSpeed(Speed s);

  // A "vertical box" which holds everything in our window
  Gtk::VBox m_vbox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar m_menubar;
  // Each menu itself
  Gtk::Menu m_menu_app;
  Gtk::Menu m_menu_draw_mode;
  Gtk::Menu m_menu_speed;
  Gtk::Menu m_menu_buffering;
  
  sigc::connection m_timerConnection;
  
  // The main OpenGL area
  Viewer m_viewer;
};

#endif
