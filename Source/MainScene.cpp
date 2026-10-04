/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "MainScene.h"

using namespace ax;


// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    // DEBUG: Log dt every frame
    scheduleUpdate();

    return true;
}

void MainScene::update(float delta)
{
    Scene::update(delta);
    AXLOGD("{}", delta);
}