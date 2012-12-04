// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#include "level.hpp"

enum Key {
    MoveForward = 0,
    MoveBackward,
    MoveLeft,
    MoveRight,

    NUM_KEYS
};

class Game {
public:
    Game(int level);
    void update(double dt);
    Level& getLevel() { return level; }
    const Segment* getPlayerSeg() const { return playerSeg; }
    double getPlayerT() const { return playerT; }
    int getPlayerSide() const { return playerSide; }
    double getPlayerTT() const { return playerTT; }
    int getPlayerSide2() const { return playerSide2; }
    double getPlayerWheel() const { return playerWheel; }

    void setKey(Key key, bool pressed);
private:
    Level level;
    Segment* playerSeg;
    double playerT;     // t of player's position
    int playerSide;     // 0, 1, 2, 3 => which side relative to the normal
    int playerSide2;    // for T-shapes
    double playerTT;    // for T-shapes
    int playerDir;      // current direction relative to the curves orientation
    double playerWheel; // angle of the wheels

    bool keyStates[NUM_KEYS];
};
