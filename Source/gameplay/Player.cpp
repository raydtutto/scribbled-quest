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

    auto rect = DrawNode::create();
    addChild(rect);
    rect->drawRect({0, 0}, {60, 60}, Color32::blue, 10.0f);

    return true;
}