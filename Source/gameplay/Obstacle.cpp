#include "Obstacle.h"
#include "axmol/2d/DrawNode.h"

using namespace ax;

Obstacle* Obstacle::create()
{
    Obstacle* pRet = new Obstacle();
    if (pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }

    delete pRet;
    pRet = nullptr;
    return nullptr;
}

bool Obstacle::init()
{
    return true;

    // if (!Node::init())
    // {
    //     return false;
    // }
    //
    // auto rect = DrawNode::create();
    // addChild(rect);
    // rect->drawRect({0,0}, {60,60}, Color32::green, 10.0f);
    //
    // return true;
}