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

void Player::setDirection(int direction)
{
    _velocity.x = direction * _movementSpeed;

    // Flip node
    if (direction != 0)
    {
        if (direction == 1)
        {
            _drawRect->setPosition(0.f, 0.f);
        }
        else
        {
            _drawRect->setPosition(getContentSize().x, 0.f);
        }
        _drawRect->setScaleX(direction);
    }
}
