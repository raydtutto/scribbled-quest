#include "Enemy.h"
#include "axmol/2d/DrawNode.h"

using namespace ax;

Enemy* Enemy::create()
{
    Enemy* pRet = new Enemy();
    if (pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }

    delete pRet;
    pRet = nullptr;
    return nullptr;
}

bool Enemy::init()
{
    if (!Node::init())
    {
        return false;
    }

    auto rect = DrawNode::create();
    addChild(rect);
    rect->drawRect({0, 0}, {60, 60}, Color32::red, 10.0f);

    return true;
}