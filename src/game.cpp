#include "game.hpp"
#include <iostream>
using namespace std;

Game::Game() {
    for (size_t i = 0; i < NUM_KEYS; i++)
        keyStates[i] = false;

    Vector3D c1[] = { Vector3D(-1, 0, 0),
                      Vector3D(0, 0, 0),
                      Vector3D(-1, 1, 0),
                      Vector3D(0, 1, 0) };
    Vector3D c2[] = { Vector3D(0, 1, 0),
                      Vector3D(1, 1, 0) };
    Vector3D c3[] = { Vector3D(1, 1, 1),
                      Vector3D(0, 1, 1),
                      Vector3D(1, 1, 2),
                      Vector3D(-1, 1, 2) };

    BezierSegment* seg1 = new BezierSegment(c1, M_PI*0.5);
    TSegment* seg2 = new TSegment(Vector3D(0, 1, 0), Vector3D(1, 0, 0));
    //StraightSegment* seg2 = new StraightSegment(c2, 0.1);
    //StraightSegment sSeg3(c2, 0.5);
    //BezierSegment bSeg3(c3, M_PI*0.5);
    seg1->next = seg2;
    level.segments.push_back(seg1);
    seg2->prev = seg1;
    seg2->side[0] = (Segment*)1; // XXX
    //sSeg2.next = &bSeg3;
    level.segments.push_back(seg2);
    //bSeg3.prev = &sSeg2;
    //level.segments.push_back(&bSeg3);

    level.calc();

    playerSeg = seg1;
    playerT = 0.1;
    playerSide = 0;
}

void Game::update(double dt) {
    //cout << "Game::update()" << endl;
    double move = dt * 0.8;

    if (keyStates[MoveForward])
        playerT += move;
    else if (keyStates[MoveBackward])
        playerT -= move;

    if (playerT > 1) {
        if (playerSeg->next == NULL) {
            playerT = 1;
        }
        else {
            playerSeg = playerSeg->next;
            playerT -= 1;
        }
    }
    else if (playerT < 0) {
        if (playerSeg->prev == NULL) {
            playerT = 0;
        }
        else {
            playerSeg = playerSeg->prev;
            playerT += 1;
        }
    }
}

void Game::setKey(Key key, bool pressed) {
    keyStates[key] = pressed;
}

