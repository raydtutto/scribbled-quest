#include "Player.h"
#include "axmol/2d/DrawNode.h"

using namespace ax;

Player* Player::create()
{
    Player* pRet = new Player();
    if (pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }

    delete pRet;
    pRet = nullptr;
    return nullptr;
}

bool Player::init()
{
    if (!Node::init())
    {
        return false;
    }

    setContentSize({60.f, 60.f});

    _drawRect = DrawNode::create();
    addChild(_drawRect);
    _drawRect->drawRect({0, 0}, getContentSize(), Color32::blue, 10.0f);
    _drawRect->drawSolidCircle({50, 50}, 5.f, Color32::red);

    return true;
}

void Player::applyActionInput(const sInputState& inputState)
{
    // Set direction: right = 1, left = -1, none = 0
    _direction = 0;
    if (inputState.isRightHeld && inputState.isLeftHeld)
    {
        _direction = 0;
    }
    else if (inputState.isRightHeld)
    {
        _direction = 1;
    }
    else if (inputState.isLeftHeld)
    {
        _direction = -1;
    }
    updateVelocity();
    flipNode();
}

void Player::updateVelocity()
{
    _velocity.x = _direction * _movementSpeed;
}

void Player::flipNode()
{
    if (_direction != 0)
    {
        if (_direction == 1)
        {
            _drawRect->setPosition(0.f, 0.f);
        }
        else
        {
            _drawRect->setPosition(getContentSize().x, 0.f);
        }
        _drawRect->setScaleX(_direction);
    }
}
