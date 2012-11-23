#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include "algebra.hpp"
#include <list>
#include <vector>

// get the point on the Bezier curve
//Vector3D bezierPoint(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3);
// get the direction of the curve (first derivative)
//Vector3D bezierDir(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3);
// get the normal of the plane the line is lying on at a given point
//Vector3D bezierNormal(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3);

enum SegmentType {
    BEZIER,
    STRAIGHT,
    T
};

const size_t nBezierSegments = 21;

class Segment {
public:
    virtual SegmentType getType() const = 0;
    Segment() : next (NULL), prev (NULL), a_begin (0) {}
    virtual ~Segment() {}
    Segment* next; // can be NULL
    Segment* prev; // can be NULL
    double a_begin;
};

class TwistSegment : public Segment {
public:
    TwistSegment() : a (0) {}
    double a; // amount of twist
};

class BezierSegment : public TwistSegment {
public:
    BezierSegment(const Vector3D curve[4]);
    SegmentType getType() const { return BEZIER; }

    Vector3D p[nBezierSegments+1];
    Vector3D d[nBezierSegments+1];
    Vector3D n[nBezierSegments+1];
private:
    void calc();
    Vector3D c[4];
};

class StraightSegment : public TwistSegment {
public:
    SegmentType getType() const { return STRAIGHT; }
    Vector3D p0, p1;
};

class TSegment : public Segment {
public:
    TSegment() { side[0] = NULL; side[1] = NULL; side[2] = NULL; side[3] = NULL; }
    SegmentType getType() const { return T; }
    Vector3D p0, p1;
    Segment* side[4]; // each can be null
};

class Level {
public:
    std::list<Segment*> segments;
    void calc();
private:
};

#endif
