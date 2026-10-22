#pragma once

#include "axmol/axmol.h"
#include "gameplay/InputState.h"

class Player : public ax::Node
{
public:
    static Player* create();

    bool init() override;
    ax::Vec2 getVelocity() const { return _velocity; }
    void applyActionInput(const sInputState& inputState);
    int getDirection() const { return _direction; }

    Player() {}
    ~Player() {}

private:
    void updateVelocity();
    void flipNode();

    float _movementSpeed = 240;
    ax::Vec2 _velocity;
    int _direction = 0;
    ax::DrawNode* _drawRect = nullptr;
};
