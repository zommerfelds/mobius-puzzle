// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

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
// would be nice to have as a symbolic formula:
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

    Vector3D ret = c1 * n(i1) + c2 * n(i2);
    ret.normalize();
    return ret;
}

Vector3D Segment::d(double t) const {
    double c1, c2;
    size_t i1, i2;
    coef(t, num(), i1, i2, c1, c2);

    Vector3D ret = c1 * d(i1) + c2 * d(i2);
    ret.normalize();
    return ret;
}

BezierSegment::BezierSegment(const Vector3D curve[4], double a)
: TwistSegment(a) {
    for (size_t i = 0; i < 4; i++)
        c[i] = curve[i];
    calcUntwisted();
}

Vector3D BezierSegment::p(double t) const {
    return bezierPoint(t, c[0], c[1], c[2], c[3]);
}

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


TSegment::TSegment(const Vector3D& start, const Vector3D& dir)
: start (start), dir (dir) {
    side[0] = NULL; side[1] = NULL; side[2] = NULL; side[3] = NULL;
}

Vector3D TSegment::p(double t) const {
    return start + t * tLenght * dir;
}

void TSegment::calc(const Vector3D& n_begin) {
    this->n_begin = n_begin;
}

bool isNear(const Vector3D& v1, const Vector3D& v2) {
    return (v1 - v2).length2() < 0.001;
}

// This function is one of the core parts for the calculation of the curves
//
void Level::calcRec(Segment* seg, const Vector3D& n_end, const Vector3D& p_end) {
    //cout << "\n== In calcRec ==" << endl;

    //cout << "p_end = " << p_end << "; p(0) = " << seg->p((size_t)0) << endl;

    size_t indexStart, indexEnd;
    size_t b;
    Segment* next, * prev;
    if ((p_end - seg->p((size_t)0)).length2() < 0.001) {
        //cout << "> regular" << endl;
        indexStart = 0;
        indexEnd = seg->num() - 1;
        next = seg->adj[1];
        prev = seg->adj[0];
        b = 0;
    } else {
        //cout << "> switched" << endl;
        indexStart = seg->num() - 1;
        indexEnd = 0;
        next = seg->adj[0];
        prev = seg->adj[1];
        b = 1;
    }

    if (seg->visited) {
        //cout << "- already visited" << endl;
        Vector3D n_begin = seg->n(indexStart);
        //cout << "n_begin = " << n_begin << endl;
        //cout << "n_end = " << n_end << endl;
        //cout << "n_begin.dot(n_end) = " << n_begin.dot(n_end) << endl;
        double angle = acos(n_begin.dot(n_end));
        if (n_begin.cross(n_end).dot(seg->d(indexStart)) < 0) // XXX see below
            angle *= -1;
        //cout << "angle: " << angle/M_PI*180 << endl;

        //cout << "num = " << angle + M_PI*2 << "; denom = " << M_PI*0.5 << endl;
        double mod = fmod(angle + M_PI*2 + 0.05, M_PI*0.5);
        //cout << "mod = " << mod << endl;
        if (mod > 0.2)
            assert(0); // angles not matching

        double div = angle/(M_PI*0.5);
        int divI = (int)(div + 4.5) % 4;
        //cout << "div = " << div << "; divI = " << divI << endl;
        seg->sideDiff[b] = divI;

        //cout << "sideDiff[0] = " << seg->sideDiff[0] << endl
        //     << "sideDiff[1] = " << seg->sideDiff[1] << endl;

        return;
    }

    //cout << "- not visited" << endl;

    seg->visited = true;

    if (seg->adj[0])
        seg->switched[0] = isNear(seg->adj[0]->p((size_t)0), seg->p((size_t)0));
    if (seg->adj[1])
        seg->switched[1] = !isNear(seg->adj[1]->p((size_t)0), seg->p(seg->num()-1));

    //cout << "switched[0] = " << seg->switched[0] << endl;
    //cout << "switched[1] = " << seg->switched[1] << endl;

    Vector3D new_n_end = n_end;

    if (seg->getType() == BEZIER) {
        BezierSegment* bSeg = static_cast<BezierSegment*>(seg);
        //bSeg->calcUntwisted();

        const Vector3D& n_begin = bSeg->n(indexStart);

        //cout << "n_begin = " << n_begin << ", n_end = " << n_end << endl;

        //cout << "n_begin.dot(n_end) = " << n_begin.dot(n_end) << endl;
        double a_begin = acos(n_begin.dot(n_end)*0.9999); // the 0.99... is for NAN problems
        if (n_begin.cross(n_end).dot(bSeg->d(indexStart)) < 0) // XXX ^ seg->switched
            a_begin *= -1;

        if (b == 1)
            a_begin -= bSeg->a;

        //cout << "a_begin = " << a_begin << endl;
        bSeg->calcTwist(a_begin);

        new_n_end = bSeg->n(indexEnd);
    } else if (seg->getType() == STRAIGHT) {
        StraightSegment* sSeg = static_cast<StraightSegment*>(seg);
        sSeg->calc(n_end);
    } else if (seg->getType() == T) {
        TSegment* tSeg = static_cast<TSegment*>(seg);
        tSeg->calc(n_end);
    }

    seg->sideDiff[0] = 0;
    seg->sideDiff[1] = 0;

    if (next != NULL)
        calcRec(next, new_n_end, seg->p(indexEnd));
    if (prev != NULL)
        calcRec(prev, n_end, seg->p(indexStart));
}

void Level::calc() {
    //cout << "Calculating level" << endl;

    if (segments.empty())
        return;

    Segment* seg = segments.front();

    assert (seg->getType() == BEZIER);
    Vector3D n_end = seg->n((size_t)0);
    Vector3D p_end = seg->p((size_t)0);

    calcRec(seg, n_end, p_end);
}
