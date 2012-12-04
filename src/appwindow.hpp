// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <gtkmm.h>
#include "viewer.hpp"
#include "game.hpp"

class AppWindow : public Gtk::Window {
public:
  AppWindow();
  
protected:
  virtual bool on_key_press_event( GdkEventKey *ev );
  virtual bool on_key_release_event( GdkEventKey *ev );

private:

  bool handleKey(GdkEventKey* ev);

  enum Speed { SLOW=0, MEDIUM, FAST };

  bool update();
  void setLevel(int l);

  // A "vertical box" which holds everything in our window
  Gtk::VBox vbox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar menubar;
  // Each menu itself
  Gtk::Menu menuApp;
  Gtk::Menu menuLevel;
  Gtk::Menu menuOptions;
  
  sigc::connection timerConnection;
  
  Game* game;

  // The main OpenGL area
  Viewer viewer;
};

#endif
