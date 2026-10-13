#pragma once

#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/scene/Node.h"
#include <climits>

class Player;

enum class eGameLayerType
{
    PLAYER,
    ENEMY,
    WORLD_OBJECT,
    OBSTACLE,
    NONE
};

enum class eZOrder
{
    DEBUG_DRAWING = INT_MAX
};

struct sSpawnEntity
{
    eGameLayerType type;
    ax::Node* node = nullptr;
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
    bool loadEntities();
    bool loadObstacles();
    bool addEntityToLayer(ax::Node* node) const;
    void initKeyboard();
    void onKeyboardInput(ax::KeyboardEvent::KeyCode keyCode, bool isPressed);

    // Current level name
    std::string _currentLevelName;

    // Tiled map with layers
    ax::FastTMXTiledMap* _tmxMap = nullptr;

    // Tiled layer with objects
    ax::FastTMXLayer* _gameplayObjects = nullptr;

    // Player
    Player* _player = nullptr;

    // List of entities on map
    std::vector<sSpawnEntity> _spawnEntity;
    std::vector<ax::Rect> _collisionList;
};
