#include "level.hpp"
#include <iostream>
#include <boost/foreach.hpp>
#include <cassert>
using namespace std;

// get the point on the Bezier curve
Vector3D bezierPoint(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3) {
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    return (uuu * p0) + (3 * uu * t * p1) + (3 * u * tt * p2) + (ttt * p3);
}
// get the direction of the curve (first derivative)
Vector3D bezierDir(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3) {
    Vector3D d = ((6-3*t)*t-3)*p0 + (t*(9*t-12)+3)*p1 + t*(t*(3*p3-9*p2)+6*p2);
    d.normalize();
    return d;
}
// would be nice to have as a symbolic formula
// get the normal of the plane the line is lying on at a given point
//Vector3D bezierNormal(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3);
// (currently, it just approximates the normals by walking through the line in small steps)

void coef(double t, size_t num, size_t& i1, size_t& i2, double& c1, double& c2) {
    double i = (t * (num-1));
    i1 = i - 0.5;
    i2 = i + 0.5;

    c2 = i - i1;
    c1 = 1 - c2;

    assert(i1 >= 0);
    assert(i2 < num);
}

Vector3D Segment::p(double t) const {
    double c1, c2;
    size_t i1, i2;
    coef(t, num(), i1, i2, c1, c2);
    return c1 * p(i1) + c2 * p(i2);
}

Vector3D Segment::n(double t) const {
    double c1, c2;
    size_t i1, i2;
    coef(t, num(), i1, i2, c1, c2);
    cout << "i1 = " << i1 << "; i2 = " << i2 << endl;
    cout << "c1 = " << c1 << "; c2 = " << c2 << "; c1 + c2 = " << c1 + c2 << endl;

    return c1 * n(i1) + c2 * n(i2);
}

Vector3D Segment::d(double t) const {
    double c1, c2;
    size_t i1, i2;
    coef(t, num(), i1, i2, c1, c2);
    return c1 * d(i1) + c2 * d(i2);
}

BezierSegment::BezierSegment(const Vector3D curve[4], double a)
: TwistSegment(a) {
    for (size_t i = 0; i < 4; i++)
        c[i] = curve[i];
}

Vector3D BezierSegment::p(double t) const {
    return bezierPoint(t, c[0], c[1], c[2], c[3]);
}

/*Vector3D BezierSegment::n(double t) const {
    size_t i = (t * (num()-1) + 0.5);
    assert(i >= 0);
    assert(i < num());
    return n(i);
}*/

Vector3D BezierSegment::d(double t) const {
    return bezierDir(t, c[0], c[1], c[2], c[3]);
}

void BezierSegment::calcUntwisted() {
    Vector3D q1 = bezierPoint(-1 / (double) nBezierSegments, c[0], c[1], c[2], c[3]);
    Vector3D q2 = bezierPoint(1 + 1 / (double) nBezierSegments, c[0], c[1], c[2], c[3]);

    for (int i = 0; i <= (int) nBezierSegments; i++) {
        double t = i / (double) nBezierSegments;
        _p[i] = bezierPoint(t, c[0], c[1], c[2], c[3]);
        _d[i] = bezierDir(t, c[0], c[1], c[2], c[3]);
    }

    Vector3D n_old;
    for (int i = 0; i <= (int) nBezierSegments; i++) {
        Vector3D* p_prev, * p_next;
        if (i == 0)
            p_prev = &q1;
        else
            p_prev = &_p[i-1];
        if (i == (int)nBezierSegments)
            p_next = &q2;
        else
            p_next = &_p[i+1];

        _n[i] = (*p_prev-_p[i]).cross(_p[i]-*p_next);
        _n[i].normalize();
        if (n_old.dot(_n[i]) < 0)
            _n[i] = -_n[i];
        n_old = _n[i];
    }
}

void BezierSegment::calcTwist(double a_begin) {
    for(size_t i = 0; i < num(); i++) {
        double t = i / (double) (num() - 1); // XXX
        _n[i].rotate(_d[i], a_begin + t * a);
    }
}

StraightSegment::StraightSegment(const Vector3D ends[2], double a)
: TwistSegment(a),
  len (0)
{
    for (size_t i = 0; i < 2; i++)
        c[i] = ends[i];
}

Vector3D StraightSegment::p(size_t i) const {
    double t = i / (double) (num() - 1); // XXX
    return c[0] + t * len * _d;
}

void StraightSegment::calc(const Vector3D& n_begin) {
    _d = c[1] - c[0];
    len = _d.normalize();
    for(size_t i = 0; i < num(); i++) {
        double t = i / (double) (num() - 1); // XXX
        _n[i] = n_begin;
        _n[i].rotate(_d, t * a);
    }
}


TSegment::TSegment(const Vector3D ends[2])
: len (0) {
    side[0] = NULL; side[1] = NULL; side[2] = NULL; side[3] = NULL;
    for (size_t i = 0; i < 2; i++)
        c[i] = ends[i];
}

void TSegment::calc(const Vector3D& n_begin) {
    this->n_begin = n_begin;
    _d = c[1] - c[0];
    len = _d.normalize();
}

void Level::calc() {
    cout << "Calculating level" << endl;

    if (segments.empty())
        return;

    Segment* seg = segments.front();
    Vector3D n_end;

    while (seg != NULL) {
        if (seg->getType() == BEZIER) {
            BezierSegment* bSeg = static_cast<BezierSegment*>(seg);
            bSeg->calcUntwisted();

            const Vector3D& n_begin = bSeg->n((size_t)0);

            if (seg->prev == NULL) {
                n_end = n_begin;
            }

            cout << "n_begin = " << n_begin << ", n_end = " << n_end << endl;

            double a_begin = acos(n_begin.dot(n_end));
            if (n_begin.cross(n_end).dot(bSeg->d((size_t)0)) < 0)
                a_begin *= -1;

            bSeg->calcTwist(a_begin);

            n_end = bSeg->n(bSeg->num() - 1);

            cout << "a_begin = " << a_begin << endl;
        } else if (seg->getType() == STRAIGHT) {
            StraightSegment* sSeg = static_cast<StraightSegment*>(seg);
            if (seg->prev == NULL) {
                n_end = (sSeg->c[1] - sSeg->c[0]).cross(Vector3D(1,0,0));
                cout << "WARNING: not really implemented" << endl;
            }
            sSeg->calc(n_end);
        } else if (seg->getType() == T) {
            TSegment* tSeg = static_cast<TSegment*>(seg);
            if (seg->prev == NULL) {
                n_end = (tSeg->c[1] - tSeg->c[0]).cross(Vector3D(1,0,0));
                cout << "WARNING: not really implemented" << endl;
            }
            tSeg->calc(n_end);
        }

        seg = seg->next;
    }
}
