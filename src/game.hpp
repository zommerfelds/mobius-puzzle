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
    double playerT;
    int playerSide;
    int playerSide2;
    double playerTT;
    int playerDir;
    double playerWheel;

    bool keyStates[NUM_KEYS];
};
