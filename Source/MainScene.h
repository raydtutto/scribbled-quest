#pragma once

#include "axmol/axmol.h"

class MainScene : public ax::Scene
{

public:
    bool init() override;
    void update(float delta) override;

    MainScene() {}
    ~MainScene() override {}
};
