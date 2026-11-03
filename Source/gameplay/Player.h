#pragma once

#include "axmol/axmol.h"
#include "gameplay/InputState.h"

class Player : public ax::Node
{
public:
    static Player* create();

    bool init() override;
    void applyActionInput(const sInputState& inputState);
    void updateGravity(float dt);
    void entityLanded();
    void entityIsInAir() { _onGround = false; }

    ax::Vec2 getVelocity() const { return _velocity; }
    int getDirection() const { return _direction; }
    bool isOnGround() const { return _onGround; }

    Player() {}
    ~Player() {}

private:
    void updateVelocity();
    void flipNode();

    ax::Vec2 _velocity;
    int _direction = 0;
    bool _onGround = false;
    bool _wasJumpHeld = false;
    ax::DrawNode* _drawRect = nullptr;
};
