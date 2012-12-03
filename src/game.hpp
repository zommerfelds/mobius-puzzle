#include "level.hpp"

enum Key {
    MoveForward = 0,
    MoveBackward,

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

    void setKey(Key key, bool pressed);
private:
    Level level;
    const Segment* playerSeg;
    double playerT;
    int playerSide;

    bool keyStates[NUM_KEYS];
};
