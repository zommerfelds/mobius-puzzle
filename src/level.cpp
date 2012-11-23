#include "level.hpp"
#include "polyroots.hpp"
#include <iostream>
#include <boost/foreach.hpp>
using namespace std;

Vector3D bezierPoint(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3) {
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    return (uuu * p0) + (3 * uu * t * p1) + (3 * u * tt * p2) + (ttt * p3);
}
Vector3D bezierDir(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3) {
    Vector3D d = ((6-3*t)*t-3)*p0 + (t*(9*t-12)+3)*p1 + t*(t*(3*p3-9*p2)+6*p2);
    d.normalize();
    return d;
}
/*
Vector3D bezierNormal(float t, const Vector3D& p0, const Vector3D& p1, const Vector3D& p2, const Vector3D& p3) {
    Vector3D d = bezierDir(t, p0, p1, p2, p3);
    d.normalize();
    double dt = 0.1;
    Vector3D q0 = bezierPoint(t - dt, p0, p1, p2, p3);
    Vector3D q1 = bezierPoint(t     , p0, p1, p2, p3);
    Vector3D q2 = bezierPoint(t + dt, p0, p1, p2, p3);

    Vector3D n = (q2 - q1).cross(q1 - q0);
    n.normalize();

    cout << "t = " << t;

    // p0*(6-6*t) + p1*(18*t-12) + (6-18*t)*p2 + 6*t*p3

    // t * (p0 - 3*p1 + 3*p2 - p3) = p0 -2*p1 + p2

    // if the curve has an inflection point, flip the normals
    if (t > 0.5 && (p1 - p0).cross(p2 - p0).dot((p2 - p0).cross(p3 - p0)) < 0) {
        n = -n;
        cout << " !";
    }
    cout << " n = " << n;

    cout << endl;


    return n;
}*/

BezierSegment::BezierSegment(const Vector3D curve[4]) {
    for (size_t i = 0; i < 4; i++)
        c[i] = curve[i];
    calc();
}

void BezierSegment::calc() {

    Vector3D q1 = bezierPoint(-1 / (double) nBezierSegments, c[0], c[1], c[2], c[3]);
    Vector3D q2 = bezierPoint(1 + 1 / (double) nBezierSegments, c[0], c[1], c[2], c[3]);

    for (int i = 0; i <= (int) nBezierSegments; i++) {
        double t = i / (double) nBezierSegments;
        p[i] = bezierPoint(t, c[0], c[1], c[2], c[3]);
        d[i] = bezierDir(t, c[0], c[1], c[2], c[3]);
    }

    Vector3D n_old;
    for (int i = 0; i <= (int) nBezierSegments; i++) {
        Vector3D* p_prev, * p_next;
        if (i == 0)
            p_prev = &q1;
        else
            p_prev = &p[i-1];
        if (i == (int)nBezierSegments)
            p_next = &q2;
        else
            p_next = &p[i+1];

        n[i] = (*p_prev-p[i]).cross(p[i]-*p_next);
        n[i].normalize();
        if (n_old.dot(n[i]) < 0)
            n[i] = -n[i];
        n_old = n[i];
    }
}

void Level::calc() {
    cout << "Calculating angles" << endl;

    if (segments.empty())
        return;

    Segment* seg = segments.front();
    double a = 0;
    Vector3D n_end;
    while (seg != NULL) {
        seg->a_begin = a;
        if (seg->getType() == BEZIER) {
            BezierSegment* bSeg = static_cast<BezierSegment*>(seg);
            Vector3D& n_begin = bSeg->n[0];
            if (seg->prev != NULL)
                a += acos(n_begin.dot(n_end));
            n_end = bSeg->n[nBezierSegments];
            seg->a_begin -= a;
            a += bSeg->a;
        }

        cout << "a_begin = " << seg->a_begin << endl;

        seg = seg->next;
    }
}
