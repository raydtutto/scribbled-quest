#include "Player.h"
#include "axmol/2d/DrawNode.h"
#include <cmath>

using namespace ax;

namespace
{
constexpr ax::Vec2 kTileSize = {60.f, 60.f};
constexpr float kGravity = 6500.f;
constexpr float kJumpHeight = kTileSize.y * 3.4f;
const float kJumpSpeed = std::sqrt(kJumpHeight * (2.f * kGravity));
constexpr float kMovementSpeed = 340.f;
}  // namespace

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

    setContentSize(kTileSize);

    _drawRect = DrawNode::create();
    addChild(_drawRect);
    _drawRect->drawRect({0, 0}, getContentSize(), Color32::blue, 10.f);
    _drawRect->drawSolidCircle({kTileSize.x - 10.f, kTileSize.y - 10.f}, 5.f, Color32::red);

    return true;
}

void Player::updateGravity(float dt)
{
    _velocity.y -= kGravity * dt;
}

void Player::entityLanded()
{
    _velocity.y = 0.f;
    _onGround = true;
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

    // Jump
    if (inputState.isJumpHeld && !_wasJumpHeld && _onGround)
    {
        _onGround = false;
        _velocity.y = kJumpSpeed;
    }
    // Cut jump
    if (!inputState.isJumpHeld && _wasJumpHeld && _velocity.y > 0)
    {
        _velocity.y *= 0.4f;
    }
    _wasJumpHeld = inputState.isJumpHeld;
}

void Player::updateVelocity()
{
    _velocity.x = _direction * kMovementSpeed;
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
