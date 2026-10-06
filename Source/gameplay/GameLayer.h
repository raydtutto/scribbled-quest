#pragma once

#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/scene/Node.h"
#include "gameplay/Player.h"

enum class eGameLayerType
{
    PLAYER,
    ENEMY,
    WORLD_OBJECT,
    OBSTACLE,
    NONE
};

struct sSpawnEntity
{
    eGameLayerType type;
    ax::Node* node{nullptr};
    std::pair<int, int> spawnPos;
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
    void loadObstacles();

    // Tiled map with layers
    ax::FastTMXTiledMap* _tmxMap = nullptr;

    // entities on map
    std::vector<sSpawnEntity> _spawnEntity;
    std::vector<ax::Rect> _collisionList;
};
