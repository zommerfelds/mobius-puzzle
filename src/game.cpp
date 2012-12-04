#include "game.hpp"
#include <iostream>
using namespace std;

Game::Game() {
    for (size_t i = 0; i < NUM_KEYS; i++)
        keyStates[i] = false;

    /*Vector3D c1[] = { Vector3D(0, 0, 0),
                      Vector3D(1, 0, 0),
                      Vector3D(0, 1, 0),
                      Vector3D(1, 1, 0) };
    //Vector3D c2[] = { Vector3D(0, 1, 0),
    //                  Vector3D(1, 1, 0) };
    Vector3D c3[] = { Vector3D(1 + tLenght, 1, 0),
                      Vector3D(3 + tLenght, 1, 0),
                      Vector3D(1 + tLenght/2, 3 + tLenght/2, 0),
                      Vector3D(1 + tLenght/2, 1 + tLenght/2, 0) };

    BezierSegment* seg1 = new BezierSegment(c1, M_PI*0.0);
    TSegment* seg2 = new TSegment(Vector3D(1, 1, 0), Vector3D(1, 0, 0));
    //StraightSegment* seg2 = new StraightSegment(c2, 0.1);
    //StraightSegment sSeg3(c2, 0.5);
    BezierSegment* seg3 = new BezierSegment(c3, M_PI*0.0);
    seg1->next = seg2;
    level.segments.push_back(seg1);
    seg2->prev = seg1;
    seg2->side[3] = seg3;
    seg2->next = seg3;
    level.segments.push_back(seg2);
    seg3->prev = seg2;
    level.segments.push_back(seg3);*/

    Vector3D c1[] = { Vector3D(0, 0, 0),
                      Vector3D(1, 0, 0),
                      Vector3D(1, 1, 0),
                      Vector3D(0, 1, 0) };
    Vector3D c2[] = { Vector3D(0, 1, 0),
                      Vector3D(-1, 1, 0),
                      Vector3D(-1, 0, 0),
                      Vector3D(0, 0, 0) };
    BezierSegment* seg1 = new BezierSegment(c1, M_PI*0.25);
    BezierSegment* seg2 = new BezierSegment(c2, M_PI*0.25);
    seg1->adj[1] = seg2;
    seg1->adj[0] = seg2;
    seg2->adj[1] = seg1;
    seg2->adj[0] = seg1;
    level.segments.push_back(seg1);
    level.segments.push_back(seg2);

    /*
    Vector3D c1[] = { Vector3D(0, 0, 0),
                      Vector3D(1, 0, 0),
                      Vector3D(2, 1, 0),
                      Vector3D(3, 2, 0) };

    Vector3D c2[] = { Vector3D(3, 4, 1),
                      Vector3D(4, 4, 0),
                      Vector3D(4, 3, 0),
                      Vector3D(3, 2, 0) };
    BezierSegment* seg1 = new BezierSegment(c1, M_PI*0.0);
    BezierSegment* seg2 = new BezierSegment(c2, M_PI*0.0);
    seg1->adj[1] = seg2;
    seg2->adj[1] = seg1;
    level.segments.push_back(seg1);
    level.segments.push_back(seg2);
*/

    level.calc();

    playerSeg = seg1;
    playerT = 0.1;
    playerSide = 0;
    playerSideT = 0;
}

void Game::update(double dt) {
    //cout << "Game::update()" << endl;
    double move = dt * 0.8;
    bool curSwitched = playerSeg->isSwitched();
    if (curSwitched)
        move *= -1;

    if (keyStates[MoveForward])
        playerT += move;
    else if (keyStates[MoveBackward])
        playerT -= move;

    switch (playerSeg->getType()) {
    default:
        if (playerT > 1) {
            if (playerSeg->adj[1] == NULL) {
                playerT = 1;
            }
            else {
                playerSide = (playerSide - playerSeg->getSideDiff(1) + 4) % 4;
                playerSeg = playerSeg->adj[1];
                cout << "playerSide = " << playerSide << endl;
                if (curSwitched ^ playerSeg->isSwitched())
                    playerT = 1;
                else
                    playerT = 0;
            }
        }
        else if (playerT < 0) {
            if (playerSeg->adj[0] == NULL) {
                playerT = 0;
            }
            else {
                playerSide = (playerSide - playerSeg->getSideDiff(0) + 4) % 4;
                playerSeg = playerSeg->adj[0];
                cout << "playerSide = " << playerSide << endl;
                if (curSwitched ^ playerSeg->isSwitched())
                    playerT = 0;
                else
                    playerT = 1;
            }
        }

        break;
    case T: {
        TSegment* t = static_cast<TSegment*>(playerSeg);
        break;
    }
    }
}

void Game::setKey(Key key, bool pressed) {
    keyStates[key] = pressed;
}

