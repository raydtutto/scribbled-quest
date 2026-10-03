#pragma once


#include "axmol/axmol.h"


class GameScene : public ax::Scene
{
public:
    static GameScene* create();

    bool init() override;

    void onEnter() override;
    void onExit() override;

    GameScene() {}
    ~GameScene() override {}
};
