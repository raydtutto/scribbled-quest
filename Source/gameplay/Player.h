#pragma once

#include "axmol/axmol.h"

class Player : public ax::Node
{
public:
    static Player* create();

    bool init() override;
    void setDirection(int direction);
    ax::Vec2 getVelocity() const { return _velocity; }

    Player() {}
    ~Player() {}

private:
    float _movementSpeed = 240;
    ax::Vec2 _velocity;
    ax::DrawNode* _drawRect = nullptr;
};
