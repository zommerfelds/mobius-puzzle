// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include "algebra.hpp"
#include <vector>

enum SegmentType {
    BEZIER,
    STRAIGHT,
    T
};

const double radius = 0.15; // thickness of a normal block
const double tRadius = 0.4; // radius of curves in T-blocks
const double tLenght = 2*tRadius + 2*radius;

const size_t nBezierSegments = 21;

class Segment {
public:
    Segment() : visited (false) { switched[0] = false; switched[1] = false; adj[0] = NULL; adj[1] = NULL; }
    virtual SegmentType getType() const = 0;
    virtual ~Segment() {}

    Segment* adj[2]; // can be NULLs

    virtual size_t num() const = 0;
    virtual Vector3D p(size_t i) const = 0;
    virtual Vector3D n(size_t i) const = 0;
    virtual Vector3D d(size_t i) const = 0;

    virtual Vector3D p(double t) const;
    virtual Vector3D n(double t) const;
    virtual Vector3D d(double t) const;

    bool isSwitched(size_t i) const { return switched[i]; }
    int getSideDiff(size_t i) const { return sideDiff[i]; }

private:
    int sideDiff[2];

    bool switched[2];
    bool visited;

    friend class Level;
};

class TwistSegment : public Segment {
public:
    TwistSegment(double a) : a (a) {}
    double a; // amount of twist
};

class BezierSegment : public TwistSegment {
public:
    BezierSegment(const Vector3D curve[4], double a);
    SegmentType getType() const { return BEZIER; }

    size_t num() const { return nBezierSegments + 1; };
    Vector3D p(size_t i) const { return _p[i]; }
    Vector3D n(size_t i) const { return _n[i]; }
    Vector3D d(size_t i) const { return _d[i]; }

    Vector3D p(double t) const;
    Vector3D d(double t) const;

private:
    void calcUntwisted();
    void calcTwist(double a_begin);

    Vector3D c[4];

    Vector3D _p[nBezierSegments+1];
    Vector3D _d[nBezierSegments+1];
    Vector3D _n[nBezierSegments+1];

    friend class Level;
};

// not used currently
class StraightSegment : public TwistSegment {
public:
    StraightSegment(const Vector3D ends[2], double a);
    SegmentType getType() const { return STRAIGHT; }

    size_t num() const { return nBezierSegments + 1; }
    Vector3D p(size_t i) const;
    Vector3D n(size_t i) const { return _n[i]; }
    Vector3D d(size_t i) const { return _d; }
private:
    void calc(const Vector3D& n_begin);

    Vector3D c[2];
    Vector3D _d;
    double len;
    Vector3D _n[nBezierSegments+1];

    friend class Level;
};

class TSegment : public Segment {
public:
    TSegment(const Vector3D& start, const Vector3D& dir);
    SegmentType getType() const { return T; }

    size_t num() const { return 2; }
    Vector3D n(size_t i) const { return n_begin; }
    Vector3D p(size_t i) const { if(i == 0) return start; else return start + tLenght*dir; }
    Vector3D d(size_t i) const { return dir; }

    Vector3D p(double t) const;

    Segment* side[4]; // each can be null
private:
    void calc(const Vector3D& n_begin);

    Vector3D start, dir;
    Vector3D n_begin;

    int sideDiff[4];

    friend class Level;
};

class Level {
public:
    std::vector<Segment*> segments;
    void calc();
private:
    void calcRec(Segment* seg, const Vector3D& n_end, const Vector3D& p_end);
};

#endif
