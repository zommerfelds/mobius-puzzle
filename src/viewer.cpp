// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#include "viewer.hpp"
#include "algebra.hpp"
#include "level.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cassert>
#include <algorithm>
#include <boost/foreach.hpp>
using namespace std;

Viewer::Viewer() {
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

void drawBezierBlock(const BezierSegment& seg) {

    glColor3f(0.8, 0.8, 1);

    const double radius = 0.15;
    Vector3D q0, q1, q2, q3;
    Vector3D s0, s1, s2, s3;

    for (int i = 0; i <= (int) nBezierSegments; i++) {
        double t = i / (double) nBezierSegments;
        const Vector3D& p = seg.p[i];
        const Vector3D& d = seg.d[i];
        Vector3D n = seg.n[i];
        n.rotate(d, seg.a_begin + t*seg.a);

        Vector3D e = d.cross(n);
        e.normalize();

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
        glEnable(GL_LIGHTING);
        //}

        n = radius * n;
        e = radius * e;

        s0 = p + n + e;
        s1 = p - n + e;
        s2 = p - n - e;
        s3 = p + n - e;
        
        if (i > 0) {
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
        
        q0 = s0;
        q1 = s1;
        q2 = s2;
        q3 = s3;
    }
}

void drawLevel(const Level& level) {
    BOOST_FOREACH(Segment* segment, level.segments) {
        switch (segment->getType()) {
        case BEZIER:
        {
            const BezierSegment* bSeg = static_cast<BezierSegment*>(segment);
            drawBezierBlock(*bSeg);
            break;
        }

        case T:
        {
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

//    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_FLAT);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    gldrawable->gl_end();
}

bool Viewer::on_expose_event(GdkEventExpose*) {
    Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

    if (!gldrawable)
        return false;

    if (!gldrawable->gl_begin(get_gl_context()))
        return false;

    // Clear the screen

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Modify the current projection matrix so that we move the
    // camera away from the origin.  We'll draw the game at the
    // origin, and we need to back up to see it.

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -7.0);
    static double r = 0;
    glRotated(r, 1, 0.2, 0);
    r += 2;

    Vector3D c1[] = { Vector3D(-1, 0, 0),
                      Vector3D(0, 0, 0),
                      Vector3D(0, 1, 0),
                      Vector3D(1, 1, 0) };
    Vector3D c2[] = { Vector3D(1, 1, 0),
                      Vector3D(2, 1, 0),
                      Vector3D(2, 1, 1),
                      Vector3D(1, 1, 1) };
    Vector3D c3[] = { Vector3D(1, 1, 1),
                      Vector3D(0, 1, 1),
                      Vector3D(1, 1, 2),
                      Vector3D(-1, 1, 2) };
    Level level;
    BezierSegment bSeg1(c1);
    BezierSegment bSeg2(c2);
    BezierSegment bSeg3(c3);
    //bSeg1.a = M_PI*0.6;
    bSeg1.next = &bSeg2;
    level.segments.push_back(&bSeg1);
    //bSeg2.a = M_PI_2;
    bSeg2.prev = &bSeg1;
    bSeg2.next = &bSeg3;
    level.segments.push_back(&bSeg2);
    bSeg3.a = 0;
    bSeg3.prev = &bSeg2;
    level.segments.push_back(&bSeg3);
    level.calc();

    drawLevel(level);

    //drawSegment(Vector3D(-1, 0, 0), Vector3D(-1, 2, 0), Vector3D(1, 0, -2), Vector3D(1, 0, 0));
    // TODO test straight segment
    //drawSegment(Vector3D(-1, 0, 0), Vector3D(0, 0, 0), Vector3D(0, 1, -1), Vector3D(0, 0, -1));
    //drawSegment(Vector3D(0, 0, 0), Vector3D(0, -1, 0), Vector3D(0, -1, -1), Vector3D(0, 0, -1));

    glFlush();

    gldrawable->swap_buffers();

    gldrawable->gl_end();

    return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event) {
    Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

    if (!gldrawable)
        return false;

    if (!gldrawable->gl_begin(get_gl_context()))
        return false;

    // Set up perspective projection, using current size and aspect
    // ratio of display

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, event->width, event->height);
    gluPerspective(40.0, (GLfloat) event->width / (GLfloat) event->height, 0.1,
            1000.0);

    // Reset to modelview matrix mode

    glMatrixMode(GL_MODELVIEW);

    gldrawable->gl_end();

    return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event) {
    return true;
}

bool Viewer::on_button_release_event(GdkEventButton*) {
    return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event) {
    return true;
}
