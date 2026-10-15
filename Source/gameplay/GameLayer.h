#pragma once

#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/scene/Node.h"
#include "axmol/base/KeyboardEvent.h"
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

enum class ePlayerAction
{
    LEFT,
    RIGHT,
    JUMP,
    DOWN,
    PICKUP,
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
    bool initKeyboard();
    void onKeyboardInput(ax::KeyboardEvent::KeyCode keyCode, bool isPressed);

    // Current level name
    std::string _currentLevelName;

    // Tiled map with layers
    ax::FastTMXTiledMap* _tmxMap = nullptr;

    // Tiled layer with objects
    ax::FastTMXLayer* _gameplayObjects = nullptr;

    // List of entities on map
    std::vector<sSpawnEntity> _spawnEntity;
    std::vector<ax::Rect> _collisionList;

    // Player
    Player* _player = nullptr;
    bool _isHeldKeyLeft = false;
    bool _isHeldKeyRight = false;
    bool _isHeldKeyJump = false;
    bool _isHeldKeyDown = false;
    bool _isHeldKeyPickUp = false;
};
