#pragma once

#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/scene/Node.h"

enum class eGameLayerType
{
    PLAYER, ENEMY, CHEST, STAR, KEY, NONE
};

struct sSpawnEntity
{
    eGameLayerType type;
    std::pair<int, int> pos;
    ax::Node* node{nullptr};
};


class GameLayer : public ax::Node
{
public:
    // Create level
    static GameLayer* create(const std::string& levelName);

    GameLayer() {}
    ~GameLayer() override;

private:
    bool loadLevel(const std::string& levelName);
    void loadEntities();

    // Tiled map with layers
    ax::FastTMXTiledMap* _tmxMap = nullptr;

    // entities on map
    std::map<std::string, sSpawnEntity> _spawnEntity;
};
