#pragma once

#include "axmol/axmol.h"

class Player : public ax::Node
{
public:
    static Player* create();

    bool init() override;

    Player() {}
    ~Player() {}

private:
};
