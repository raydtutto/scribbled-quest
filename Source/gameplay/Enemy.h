#pragma once

#include "axmol/axmol.h"

class Enemy : public ax::Node
{
public:
    static Enemy* create();

    bool init() override;

    Enemy() {}
    ~Enemy() {}

private:
};