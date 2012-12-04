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
    Game();
    void update(double dt);
    Level& getLevel() { return level; }
    const Segment* getPlayerSeg() { return playerSeg; }
    double getPlayerT() { return playerT; }
    int getPlayerSide() { return playerSide; }
    double getPlayerTT() { return playerTT; }
    int getPlayerSide2() { return playerSide2; }

    void setKey(Key key, bool pressed);
private:
    Level level;
    Segment* playerSeg;
    double playerT;
    int playerSide;
    int playerSide2;
    double playerTT;
    int playerDir;

    bool keyStates[NUM_KEYS];
};
