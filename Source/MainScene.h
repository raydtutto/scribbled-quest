#pragma once

#include "Inspector/Inspector.h"
#include "axmol/axmol.h"

class MainScene : public ax::Scene
{

public:
    bool init() override;

    void onEnter() override {
        ax::Scene::onEnter();
        ax::extension::Inspector::getInstance()->openForScene(this);
    }
    void onExit() override {
        ax::extension::Inspector::getInstance()->close();
        ax::Scene::onExit();
    }


    MainScene() {}
    ~MainScene() override {}

};
