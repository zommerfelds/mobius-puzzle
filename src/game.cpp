#include "game.hpp"
#include <iostream>
using namespace std;

Game::Game(int l)
: playerDir (1),
  playerWheel (0.0),
  playerT (0.1),
  playerSide (0),
  playerSide2 (0),
  playerTT (0.0) {
    for (size_t i = 0; i < NUM_KEYS; i++)
        keyStates[i] = false;


    switch (l) {
    case 0: {

        Vector3D c1[] = { Vector3D(0, 0, 0),
                          Vector3D(2, 0, 0),
                          Vector3D(2, 1, 2),
                          Vector3D(0, 1, 2) };

    /*
        Vector3D c2[] = { Vector3D(-0, 0, 0),
                          Vector3D(-2, 0, 0),
                          Vector3D(-2, 2, 0),
                          Vector3D( 0, 2, 0) };

    */
        Vector3D c2[] = { Vector3D( 0, 1, 2),
                          Vector3D(-2, 1, 2),
                          Vector3D(-2, 0, 0),
                          Vector3D( 0, 0, 0) };

        BezierSegment* seg1 = new BezierSegment(c1, M_PI*0.25);
        BezierSegment* seg2 = new BezierSegment(c2, M_PI*0.25);
        seg1->adj[1] = seg2;
        seg1->adj[0] = seg2;
        seg2->adj[1] = seg1;
        seg2->adj[0] = seg1;
        level.segments.push_back(seg1);
        level.segments.push_back(seg2);

        playerSeg = seg1;
        break;
    }

    case 1: {
        Vector3D c1[] = { Vector3D(0, 0, 0),
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
        seg1->adj[1] = seg2;
        level.segments.push_back(seg1);
        seg2->adj[0] = seg1;
        seg2->side[3] = seg3;
        seg2->adj[1] = seg3;
        level.segments.push_back(seg2);
        seg3->adj[0] = seg2;
        seg3->adj[1] = seg2;
        level.segments.push_back(seg3);

        playerSeg = seg1;
        break;
    }
    }
    /*
    Vector3D c1[] = { Vector3D(0, 0, 0),
                      Vector3D(1, 0, 0),
                      Vector3D(1, 1, 0),
                      Vector3D(0, 1, 0) };
    Vector3D c2[] = { Vector3D(0, 1, 0),
                      Vector3D(-1, 1, 0),
                      Vector3D(-1, 0, 0),
                      Vector3D(0, 0, 0) };
    BezierSegment* seg1 = new BezierSegment(c1, M_PI*0);
    BezierSegment* seg2 = new BezierSegment(c2, M_PI*1);
    seg1->adj[1] = seg2;
    seg1->adj[0] = seg2;
    seg2->adj[1] = seg1;
    seg2->adj[0] = seg1;
    level.segments.push_back(seg1);
    level.segments.push_back(seg2);
    */

    level.calc();
}

void Game::update(double dt) {
    //cout << "Game::update()" << endl;
    double move = dt * 0.8;
    double wheelMove = dt * 1000.0;
    move *= playerDir;

    if (keyStates[MoveForward]) {
        playerT += move;
        playerWheel += wheelMove;
    } else if (keyStates[MoveBackward]) {
        playerT -= move;
        playerWheel -= wheelMove;
    }

    if (playerT > 1) {
        if (playerSeg->adj[1] == NULL) {
            playerT = 1;
        } else {
            playerSide = (playerSide - playerSeg->getSideDiff(1) + 4) % 4;
            cout << "playerSide = " << playerSide << endl;
            cout << "playerSeg->isSwitched(1) = " << playerSeg->isSwitched(1) << endl;
            if (playerSeg->isSwitched(1)) {
                playerDir *= -1;
                playerT = 1;
            } else {
                playerT = 0;
            }
            playerTT = 0;
            playerSeg = playerSeg->adj[1];
        }
    }
    else if (playerT < 0) {
        if (playerSeg->adj[0] == NULL) {
            playerT = 0;
        } else {
            playerSide = (playerSide - playerSeg->getSideDiff(0) + 4) % 4;
            cout << "playerSide = " << playerSide << endl;
            if (playerSeg->isSwitched(0)) {
                playerDir *= -1;
                playerT = 0;
            } else {
                playerT = 1;
            }
            playerTT = 0;
            playerSeg = playerSeg->adj[0];
        }
    }

    //cout << "playerT = " << playerT << endl;

    if (playerSeg->getType() == T) {

        if (keyStates[MoveRight])
            playerTT += move;
        else if (keyStates[MoveLeft])
            playerTT -= move;

        TSegment* t = static_cast<TSegment*>(playerSeg);

        if (playerTT < -0.5) {
            playerSeg = t->side[3];
            playerT = 0;
            playerTT = 0;
        }
    }
}

void Game::setKey(Key key, bool pressed) {
    keyStates[key] = pressed;
}

