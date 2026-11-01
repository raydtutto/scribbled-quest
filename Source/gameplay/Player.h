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
    void entityLanded() { _velocity.y = 0.f; }

    ax::Vec2 getVelocity() const { return _velocity; }
    int getDirection() const { return _direction; }

    Player() {}
    ~Player() {}

private:
    void updateVelocity();
    void flipNode();

    float _movementSpeed = 240;
    float _gravity = 5000.f;
    ax::Vec2 _velocity;
    int _direction = 0;
    ax::DrawNode* _drawRect = nullptr;
};
