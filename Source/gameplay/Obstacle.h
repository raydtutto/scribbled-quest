#pragma once

#include "axmol/axmol.h"

class Obstacle : public ax::Node
{
public:
    static Obstacle* create();

    bool init() override;

    Obstacle() {}
    ~Obstacle() {}

private:
};