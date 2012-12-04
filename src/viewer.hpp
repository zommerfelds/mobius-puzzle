// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include "shader.hpp"
#include "algebra.hpp"
#include "particles.hpp"
#include <gtkmm.h>
#include <gtkglmm.h>

class Game;
class Segment;
class TSegment;
class GLUquadric;

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer(Game& game);
  virtual ~Viewer();

  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();
  
  void setGame(Game& g);

  void toggleSkyBox() { enableSkyBox = !enableSkyBox; }
  void toggleParticleSystem() { enableParticleSystem = !enableParticleSystem; }
  void setGlow(int g) { glow = g; }

protected:

  // Events we implement
  // Note that we could use gtkmm's "signals and slots" mechanism
  // instead, but for many classes there's a convenient member
  // function one just needs to define that'll be called with the
  // event.

  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);

private:
  bool isGlInit;

  void createDrawBuffer();
  void loadTextures();
  void scene(bool lightAndTex);
  void drawLevel(bool lightAndTex);
  void drawCurveBlock(const Segment& s, bool lightAndTex);
  void drawTBlock(const TSegment& t, bool lightAndTex);
  void setProjAndModelViewMatrix();
  void drawSkyBox();
  void drawRobot();

  sigc::connection timerConnection;
  ShaderManager shaderMgr;

  Game* game;

  GLuint fbo[2];
  GLuint fboTex[2];
  GLuint tex[9];

  Vector3D camera;

  int oldX;
  int oldY;

  ParticleSystem* particleSys;

  bool enableSkyBox;
  bool enableParticleSystem;
  int glow;

  GLUquadric* qobj;
};

#endif
