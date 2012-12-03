#include "particles.hpp"
#include "level.hpp"
#include <boost/foreach.hpp>
#include <cstdlib>
#include <ctime>
#include <GL/gl.h>
#include <iostream>
using namespace std;

namespace {

double random01() {
    return rand() / (double)RAND_MAX;
}
}

ParticleSystem::ParticleSystem(const Level& level)
: level (level) {
    srand ( time(NULL) );

    BOOST_FOREACH(Particle& p, particles) {
        makeParticle(p);
    }
}

void ParticleSystem::makeParticle(Particle& p) {
    size_t nsegs = level.segments.size();
    size_t segnum = rand() % nsegs;
    double t = random01();
    double angle = random01() * 2 * M_PI;
    Segment* seg = level.segments[segnum];
    Vector3D d = seg->d(t);
    Vector3D n = seg->n(t);
    n.rotate(d, angle);
    p.pos = seg->p(t) + 0.17 * n;
    p.dir = n;
    p.a = 0;
    p.max = random01();
}



void ParticleSystem::update(double dt) {
    BOOST_FOREACH(Particle& p, particles) {
        p.pos = p.pos + dt * 0.2 * p.dir;
        p.a += dt * 0.5;
        if (p.a > p.max)
            makeParticle(p);
    }
}

void ParticleSystem::draw() {
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);


    BOOST_FOREACH(Particle& p, particles) {
        double c = 1-p.a/p.max;
        glColor4f(1,0,0,c);
        glVertex3dv(&p.pos[0]);
    }

    glEnd();
}
