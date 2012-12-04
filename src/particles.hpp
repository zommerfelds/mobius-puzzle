// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#ifndef PARTICLES_HPP_
#define PARTICLES_HPP_

#include "algebra.hpp"

class Level;

const int numParticles = 300;

class Particle {
public:
    Vector3D pos;
    Vector3D dir;
    double a;
    double max;
};

class ParticleSystem {
public:
    ParticleSystem(const Level& level);

    void update(double dt);
    void draw();
private:
    void makeParticle(Particle& p);

    const Level& level;
    Particle particles[numParticles];
};

#endif
