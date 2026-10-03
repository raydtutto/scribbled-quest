#include "GameScene.h"
#include "Inspector/Inspector.h"

using namespace ax;

GameScene* GameScene::create()
{
    GameScene* pRet = new GameScene();
    if (pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        AX_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto bgLayer = LayerColor::create(Color32::yellow);
    if (bgLayer)
    {
        addChild(bgLayer);
    }

    return true;
}

void GameScene::onEnter()
{
    ax::Scene::onEnter();
    ax::extension::Inspector::getInstance()->openForScene(this);
}

void GameScene::onExit()
{
    ax::extension::Inspector::getInstance()->close();
    ax::Scene::onExit();
}