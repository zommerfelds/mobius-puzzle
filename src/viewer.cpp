// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#include <GL/glew.h>
#include "viewer.hpp"
#include "algebra.hpp"
#include "level.hpp"
#include "game.hpp"
#include <iostream>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <cassert>
#include <algorithm>
#include <boost/foreach.hpp>
#include <GL/glew.h>
#include <fstream>
#include <vector>
using namespace std;

namespace {

// XXX
#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(const char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

// draws a cube (front faces are counterclockwise)
void drawCube(double length) {
  glBegin(GL_QUADS);

  length *= 0.5;

  // comments are as if X-axis goes right, Y-axis goes away from viewer, Z-axis goes up

  // front face
  glNormal3d(0.0, -1.0, 0.0);
  glVertex3d(+length, -length, -length);
  glVertex3d(+length, -length, +length);
  glVertex3d(-length, -length, +length);
  glVertex3d(-length, -length, -length);
  // back face
  glNormal3d(0.0, +1.0, 0.0);
  glVertex3d(-length, +length, -length);
  glVertex3d(-length, +length, +length);
  glVertex3d(+length, +length, +length);
  glVertex3d(+length, +length, -length);
  // right face
  glNormal3d(+1.0, 0.0, 0.0);
  glVertex3d(+length, +length, -length);
  glVertex3d(+length, +length, +length);
  glVertex3d(+length, -length, +length);
  glVertex3d(+length, -length, -length);
  // left face
  glNormal3d(-1.0, 0.0, 0.0);
  glVertex3d(-length, -length, -length);
  glVertex3d(-length, -length, +length);
  glVertex3d(-length, +length, +length);
  glVertex3d(-length, +length, -length);
  // top face
  glNormal3d(0.0, 0.0, +1.0);
  glVertex3d(-length, -length, +length);
  glVertex3d(+length, -length, +length);
  glVertex3d(+length, +length, +length);
  glVertex3d(-length, +length, +length);
  // bottom face
  glNormal3d(0.0, 0.0, -1.0);
  glVertex3d(-length, +length, -length);
  glVertex3d(+length, +length, -length);
  glVertex3d(+length, -length, -length);
  glVertex3d(-length, -length, -length);

  glEnd();
}

// draws a cube (front faces are counterclockwise)
void drawParallelepiped(const Vector3D& p0, const Vector3D& v1, const Vector3D& v2, const Vector3D& v3) {
  glBegin(GL_QUADS);

  Vector3D p1 = p0 + v1;
  Vector3D p2 = p1 + v2;
  Vector3D p3 = p0 + v2;
  Vector3D p4 = p0 + v3;
  Vector3D p5 = p1 + v3;
  Vector3D p6 = p2 + v3;
  Vector3D p7 = p3 + v3;

  Vector3D n1 = v2.cross(v1);
  Vector3D n2 = v3.cross(v2);
  Vector3D n3 = v1.cross(v3);

  // comments are as if X-axis goes right, Y-axis goes away from viewer, Z-axis goes up

  glNormal3dv(&n1[0]);
  glVertex3d(p0[0], p0[1], p0[2]);
  glVertex3dv(&p3[0]);
  glVertex3dv(&p2[0]);
  glVertex3dv(&p1[0]);

  glNormal3dv(&n2[0]);
  glVertex3d(p0[0], p0[1], p0[2]);
  glVertex3dv(&p4[0]);
  glVertex3dv(&p7[0]);
  glVertex3dv(&p3[0]);

  glNormal3dv(&n3[0]);
  glVertex3d(p0[0], p0[1], p0[2]);
  glVertex3dv(&p1[0]);
  glVertex3dv(&p5[0]);
  glVertex3dv(&p4[0]);

  glNormal3d(-n1[0], -n1[1], -n1[2]);
  glVertex3dv(&p4[0]);
  glVertex3dv(&p5[0]);
  glVertex3dv(&p6[0]);
  glVertex3dv(&p7[0]);

  glNormal3d(-n2[0], -n2[1], -n2[2]);
  glVertex3dv(&p1[0]);
  glVertex3dv(&p2[0]);
  glVertex3dv(&p6[0]);
  glVertex3dv(&p5[0]);

  glNormal3d(-n3[0], -n3[1], -n3[2]);
  glVertex3dv(&p3[0]);
  glVertex3dv(&p7[0]);
  glVertex3dv(&p6[0]);
  glVertex3dv(&p2[0]);

  glEnd();
}

}

Viewer::Viewer(Game& game)
: isGlInit (false),
  game (game),
  camera (1, 0, 0) {
    Glib::RefPtr<Gdk::GL::Config> glconfig;

    // Ask for an OpenGL Setup with
    //  - red, green and blue component colour
    //  - a depth buffer to avoid things overlapping wrongly
    //  - double-buffered rendering to avoid tearing/flickering
    glconfig = Gdk::GL::Config::create(
            Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH | Gdk::GL::MODE_DOUBLE);
    if (glconfig == 0) {
        // If we can't get this configuration, die
        std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
        abort();
    }

    // Accept the configuration
    set_gl_capability(glconfig);

    camera = Vector3D(1, 1, 1);
    camera.normalize();

    // Register the fact that we want to receive these events
    add_events(
            Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK
                    | Gdk::BUTTON3_MOTION_MASK | Gdk::BUTTON_PRESS_MASK
                    | Gdk::BUTTON_RELEASE_MASK | Gdk::VISIBILITY_NOTIFY_MASK);
}

Viewer::~Viewer() {
    // Nothing to do here right now.
}

void Viewer::invalidate() {
    //Force a rerender
    Gtk::Allocation allocation = get_allocation();
    get_window()->invalidate_rect(allocation, false);
}


void Viewer::drawCurveBlock(const Segment& s, bool lightAndTex) {

    glColor3f(0.8, 0.8, 1);
    glDisable(GL_TEXTURE_2D);

    const double radius = 0.15;
    Vector3D q0, q1, q2, q3;
    Vector3D s0, s1, s2, s3;

    for (size_t i = 0; i < s.num(); i++) {
        const Vector3D& p = s.p(i);
        Vector3D n = s.n(i);
        const Vector3D& d = s.d(i);

        Vector3D e = d.cross(n);
        e.normalize();

        double tex0 = i / (double) s.num();
        double tex1 = (i+1) / (double) s.num();

#if 1
        //if (i == 0) {
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);/*
        glColor3f(0, 1, 0);
        glVertex3dv(&p[0]);
        glVertex3d(p[0] + d[0], p[1] + d[1], p[2] + d[2]);*/
        glColor3f(0, 0, 1);
        glVertex3d(p[0], p[1], p[2]);
        glVertex3d(p[0] + n[0]/2, p[1] + n[1]/2, p[2] + n[2]/2);/*
        glColor3f(1, 0, 1);
        glVertex3dv(&p[0]);
        glVertex3d(p[0] + e[0], p[1] + e[1], p[2] + e[2]);*/
        glEnd();
        glColor3f(0.8, 0.8, 1);
        //}
#endif

        if (lightAndTex) {
            glEnable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex[0]);
        }
        else
            glDisable(GL_LIGHTING);

        glColor3f(1, 1, 1);

        n = radius * n;
        e = radius * e;

        s0 = p + n + e;
        s1 = p - n + e;
        s2 = p - n - e;
        s3 = p + n - e;

        if (i > 0) {
            glBegin(GL_QUADS);

            glColor3f(1, 0, 1);
            glNormal3dv(&e[0]);
            glTexCoord2f (tex1, 0.0);
            glVertex3dv(&s0[0]);
            glTexCoord2f (tex0, 0.0);
            glVertex3dv(&q0[0]);
            glTexCoord2f (tex0, 0.2);
            glVertex3dv(&q1[0]);
            glTexCoord2f (tex1, 0.2);
            glVertex3dv(&s1[0]);

            glColor3f(1, 1, 0);
            glNormal3d(-n[0],-n[1],-n[2]);
            glTexCoord2f (tex1, 0.0);
            glVertex3dv(&s1[0]);
            glTexCoord2f (tex0, 0.0);
            glVertex3dv(&q1[0]);
            glTexCoord2f (tex0, 0.2);
            glVertex3dv(&q2[0]);
            glTexCoord2f (tex1, 0.2);
            glVertex3dv(&s2[0]);

            glColor3f(0, 1, 1);
            glNormal3d(-e[0],-e[1],-e[2]);
            glTexCoord2f (tex1, 0.0);
            glVertex3dv(&s2[0]);
            glTexCoord2f (tex0, 0.0);
            glVertex3dv(&q2[0]);
            glTexCoord2f (tex0, 0.2);
            glVertex3dv(&q3[0]);
            glTexCoord2f (tex1, 0.2);
            glVertex3dv(&s3[0]);

            glColor3f(0, 1, 0);
            glNormal3d(n[0],n[1],n[2]);
            glTexCoord2f (tex1, 0.0);
            glVertex3dv(&s3[0]);
            glTexCoord2f (tex0, 0.0);
            glVertex3dv(&q3[0]);
            glTexCoord2f (tex0, 0.2);
            glVertex3dv(&q0[0]);
            glTexCoord2f (tex1, 0.2);
            glVertex3dv(&s0[0]);

            glEnd();
        }

        q0 = s0;
        q1 = s1;
        q2 = s2;
        q3 = s3;
    }
}

void drawTBlock(const TSegment& t) {

    glColor3f(0.8, 0.8, 1);

    const double radius = 0.15;
    Vector3D q0, q1, q2, q3;
    Vector3D s0, s1, s2, s3;

    Vector3D n = t.n(0);

    Vector3D e = t.d(0).cross(n);
    e.normalize();

    n = radius * n;
    e = radius * e;

    q0 = t.p(0) + n + e;
    q1 = t.p(0) - n + e;
    q2 = t.p(0) - n - e;
    q3 = t.p(0) + n - e;

    s0 = t.p(1) + n + e;
    s1 = t.p(1) - n + e;
    s2 = t.p(1) - n - e;
    s3 = t.p(1) + n - e;

    glBegin(GL_QUAD_STRIP);

    glNormal3dv(&e[0]);
    glVertex3dv(&s0[0]);
    glVertex3dv(&q0[0]);
    glVertex3dv(&s1[0]);
    glVertex3dv(&q1[0]);
    glNormal3d(-n[0],-n[1],-n[2]);
    glVertex3dv(&s2[0]);
    glVertex3dv(&q2[0]);
    glNormal3d(-e[0],-e[1],-e[2]);
    glVertex3dv(&s3[0]);
    glVertex3dv(&q3[0]);
    glNormal3d(n[0],n[1],n[2]);
    glVertex3dv(&s0[0]);
    glVertex3dv(&q0[0]);

    glEnd();
}

void Viewer::drawLevel(bool lightAndTex) {
    Vector3D n_end;
    BOOST_FOREACH(Segment* segment, game.getLevel().segments) {
        switch (segment->getType()) {
        case BEZIER:
            //curve = static_cast<const Curve*>(static_cast<const BezierSegment*>(segment));
            /* no break */
        case STRAIGHT:
            //curve = static_cast<const Curve*>(static_cast<const StraightSegment*>(segment));

            drawCurveBlock(*segment, lightAndTex);
            break;

        case T:
        {
            const TSegment* tSeg = static_cast<TSegment*>(segment);
            drawTBlock(*tSeg);
            break;
        }

        default:
            assert(false);
            break;
        }
    }
}

void Viewer::on_realize() {
    // Do some OpenGL setup.
    // First, let the base class do whatever it needs to
    Gtk::GL::DrawingArea::on_realize();

    Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

    if (!gldrawable)
        return;

    if (!gldrawable->gl_begin(get_gl_context()))
        return;

    // Just enable depth testing and set the background colour.
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);

    glEnable(GL_CULL_FACE); // XXX
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_FLAT);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    gldrawable->gl_end();

    glewInit();

    cout << "===> on_realize()" << endl;
    createDrawBuffer();
    loadTextures();

    list<string> defines;
    defines.push_back("VERTICAL_BLUR_9");
    shaderMgr.loadShader("glowV", "data/glow.vert", "data/glow.frag", defines);
    defines.clear();
    defines.push_back("HORIZONTAL_BLUR_9");
    shaderMgr.loadShader("glowH", "data/glow.vert", "data/glow.frag", defines);

    shaderMgr.useShader("glowH");
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, tex[0]);
    shaderMgr.setParam("blurSampler", 0);

    shaderMgr.useShader("glowV");
    //glBindTexture(GL_TEXTURE_2D, tex[1]);
    shaderMgr.setParam("blurSampler", 0);

    shaderMgr.useShader(ShaderManager::defaultShader);

    isGlInit = true;
}

void Viewer::createDrawBuffer() {

    for (size_t i = 0; i < 2; i++) {

        // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
        glGenFramebuffers(1, &fbo[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);

        glGenTextures(1, &fboTex[i]);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, fboTex[i]);

        // Give an empty image to OpenGL ( the last "0" )
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

        // Poor filtering. Needed !
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Set "renderedTexture" as our colour attachement #0
        glFramebufferTextureARB(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTex[i], 0);

        // Set the list of draw buffers.
        GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers); // "1" is the size of drawBuffers

        // Always check that our framebuffer is ok
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw runtime_error("problem with the framebuffer");
        }

    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to screen
}

void Viewer::loadTextures() {
    glGenTextures(1, &tex[0]);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, tex[0]);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    ifstream file( "data/Rainbow Shards.data", ios::binary );
    if (!file) throw runtime_error("error reading texture");
    // copies all data into buffer
    file.seekg (0, ios::end);
    size_t size = file.tellg();
    file.seekg (0, ios::beg);
    char* buffer = new char [size];
    file.read (buffer, size);
    file.close();
    cout << "size = " << size << endl;

    // Textur wird hier in die Grafikkarte geladen! Dabei werden Mipmaps generiert.
    gluBuild2DMipmaps(GL_TEXTURE_2D,                    // 2D Textur wird geladen
                      GL_RGB,
                      512,     // Breite des Bildes
                      512,    // Höhe des Bildes
                      GL_RGB,                           // Format
                      GL_UNSIGNED_BYTE,                 // Wie Daten aufgeschriben sind
                      buffer);

    delete [] buffer;
}

double r = 0;

void Viewer::scene(bool lightAndTex) {

    // Set up perspective projection, using current size and aspect
    // ratio of display

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat) get_width() / (GLfloat) get_height(), 0.1, 1000.0);

    // Modify the current projection matrix so that we move the
    // camera away from the origin.  We'll draw the game at the
    // origin, and we need to back up to see it.

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*glTranslated(0.0, 0.0, -7.0);
    glRotated(r, 1, 0.2, 0);*/

    const Segment* seg = game.getPlayerSeg();
    int side = game.getPlayerSide();
    double t = game.getPlayerT();

    Vector3D n = seg->n(t);
    Vector3D d = seg->d(t);
    n.rotate(d, side*M_PI*0.5);
    Vector3D p = seg->p(t);
    Vector3D g = p + 0.17 * n;

    Vector3D c = g + 7*camera;

    gluLookAt(c[0], c[1], c[2],
              g[0], g[1], g[2],
              n[0], n[1], n[2]);

    drawLevel(lightAndTex);
}

bool Viewer::on_expose_event(GdkEventExpose*) {

    assert(isGlInit);
    Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

    if (!gldrawable)
        return false;

    if (!gldrawable->gl_begin(get_gl_context()))
        return false;

    // Clear the screen

    printOpenGLError();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_TEXTURE_2D);
    glViewport(0, 0, 512, 512);
    //glViewport(0, 0, get_width(), get_height());
    shaderMgr.useShader(ShaderManager::defaultShader);

    printOpenGLError();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene(false);
/*
    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport );
    int width  = m_viewport[2];
    int height =  m_viewport[3];

    glBindTexture(GL_TEXTURE_2D, renderedTexture);


    // rgb image
    glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,m_viewport[0],
                    m_viewport[1], m_viewport[2], m_viewport[3],0);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    char* raw_img = new char [width * height * 3];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_img);

    ofstream fout ("img",ios_base::binary);
    fout.write(raw_img, width * height * 3);
    delete raw_img;*/
    //exit(0);
/*
    static int count = 0;
    count++;
    if (count == 10)
        exit(0);*/


    glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
    glViewport(0, 0, 512, 512);
    //glViewport(0, 0, get_width(), get_height());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    shaderMgr.useShader("glowH");
    //glColor4f(1.0f,1.0f,0,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, fboTex[0]);
    glBegin(GL_QUADS);
    glTexCoord2f (0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f (1.0, 0.0);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f (1.0, 1.0);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f (0.0, 1.0);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, get_width(), get_height());

    shaderMgr.useShader("glowV");
    //glColor4f(1.0f,1.0f,0,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, fboTex[1]);
    glBegin(GL_QUADS);
    glTexCoord2f (0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f (1.0, 0.0);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f (1.0, 1.0);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f (0.0, 1.0);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    shaderMgr.useShader(ShaderManager::defaultShader);
    glClear(GL_DEPTH_BUFFER_BIT);

    scene(true);

    const Segment* seg = game.getPlayerSeg();
    int side = game.getPlayerSide();
    double t = game.getPlayerT();

    Vector3D n = seg->n(t);
    Vector3D d = seg->d(t);
    n.rotate(d, side*M_PI*0.5);
    Vector3D p = seg->p(t);
    Vector3D e = d.cross(n);
    Vector3D p0 = p + 0.17 * n - 0.1*d - 0.1*e;

    //glTranslated(center[0], center[1], center[2]);
    //glRotated(30, 0, 0, 1);
    glColor3f(0, 0, 1);
    //drawCube(0.2);
    drawParallelepiped(p0, 0.2*d, 0.2*n, 0.2*e);
    //drawParallelepiped(center, Vector3D(0.2,0,0), Vector3D(0,0,-0.2), Vector3D(0,0.2,0));

    glFlush();

    gldrawable->swap_buffers();

    gldrawable->gl_end();

    r += 0.5;
    cout << "r = " << r << endl;

    /*
    // update camera
    int x, y;
    get_pointer(x, y);
    int dx = x - oldX;
    int dy = y - oldY;
    cout << "dx = " << dx << ", dy = " << dy << endl;
    camera.rotate(n, dx*0.01);
    camera.normalize();
    cout << "camera: " << camera << endl;
    oldX = x;
    oldY = y;
    */

    return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event) {
    Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

    get_pointer(oldX, oldY);

    if (!gldrawable)
        return false;

    if (!gldrawable->gl_begin(get_gl_context()))
        return false;

    gldrawable->gl_end();

    return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event) {
    oldX = event->x;
    oldY = event->y;
    return true;
}

bool Viewer::on_button_release_event(GdkEventButton*) {
    return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event) {
    int dx = event->x - oldX;
    int dy = event->y - oldY;
    Vector3D n = game.getPlayerSeg()->n(game.getPlayerT());
    camera.rotate(n, dx*0.01);
    camera.rotate(n.cross(camera), dy*0.01);
    camera.normalize();
    oldX = event->x;
    oldY = event->y;
    return true;
}
