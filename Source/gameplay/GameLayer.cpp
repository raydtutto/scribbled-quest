#include "GameLayer.h"

#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/2d/DrawNode.h"
#include "fmt/chrono.h"
#include "gameplay/Obstacle.h"
#include "gameplay/Player.h"
#include "gameplay/Enemy.h"
#include "gameplay/WorldObject.h"

// Third-party libraries
#include "magic_enum/magic_enum_all.hpp"

// Debug only
#define DEBUG_DRAW          1
#define DEBUG_ACTION        1
#define DEBUG_ACTION_UPDATE 0

using namespace ax;

namespace
{
std::string getTypeName(eGameLayerType type)
{
    switch (type)
    {
    case eGameLayerType::PLAYER:
        return "player";
        break;
    case eGameLayerType::ENEMY:
        return "enemy";
        break;
    case eGameLayerType::WORLD_OBJECT:
        return "world_object";
        break;
    case eGameLayerType::OBSTACLE:
        return "obstacle";
        break;
    default:
        return "";
        break;
    }
    return "";
}

eGameLayerType getEntityType(const std::string& typeName)
{
    if (typeName == "player")
        return eGameLayerType::PLAYER;
    if (typeName == "enemy")
        return eGameLayerType::ENEMY;
    if (typeName == "world_object")
        return eGameLayerType::WORLD_OBJECT;
    if (typeName == "obstacle")
        return eGameLayerType::OBSTACLE;

    return eGameLayerType::NONE;
}

ePlayerAction getPlayerAction(ax::KeyboardEvent::KeyCode keyCode)
{
    using EventKeyCode = KeyboardEvent::KeyCode;

    switch (keyCode)
    {
    case EventKeyCode::KEY_LEFT_ARROW:
    case EventKeyCode::KEY_A:
        return ePlayerAction::LEFT;
        break;
    case EventKeyCode::KEY_RIGHT_ARROW:
    case EventKeyCode::KEY_D:
        return ePlayerAction::RIGHT;
        break;
    case EventKeyCode::KEY_UP_ARROW:
    case EventKeyCode::KEY_W:
    case EventKeyCode::KEY_SPACE:
        return ePlayerAction::JUMP;
        break;
    case EventKeyCode::KEY_DOWN_ARROW:
    case EventKeyCode::KEY_S:
        return ePlayerAction::DOWN;
        break;
    case EventKeyCode::KEY_E:
        return ePlayerAction::PICKUP;
        break;
    default:
        return ePlayerAction::NONE;
        break;
    }
    return ePlayerAction::NONE;
}

std::pair<int, int> getTilePosition(const Vec2 tileSize, const int tileX, const int tileY)
{
    return {ceil(tileX / tileSize.x) - 1, ceil(tileY / tileSize.y) - 1};
}
}  // namespace

GameLayer* GameLayer::create(const std::string& levelName)
{
    GameLayer* pRet = new GameLayer();
    if (pRet->init() && pRet->loadLevel(levelName) && pRet->initKeyboard())
    {
        pRet->scheduleUpdate();
        pRet->autorelease();
        return pRet;
    }

    delete pRet;
    pRet = nullptr;
    return nullptr;
}

void GameLayer::update(float dt)
{
    ax::Node::update(dt);

    // Set direction: right = 1, left = -1, none = 0
    int direction = 0;
    if (_inputActions[ePlayerAction::RIGHT] > 0 && _inputActions[ePlayerAction::LEFT] > 0)
    {
        direction = 0;
    }
    else if (_inputActions[ePlayerAction::RIGHT] > 0)
    {
        direction = 1;
    }
    else if (_inputActions[ePlayerAction::LEFT] > 0)
    {
        direction = -1;
    }
    _player->setDirection(direction);

    // Move entity
    const auto position = _player->getPosition();
    _player->setPosition(position + _player->getVelocity() * dt);

#if DEBUG_ACTION_UPDATE
    AXLOGD("Player's velocity {} and direction {}", _player->getVelocity().x, direction);
#endif
}

GameLayer::~GameLayer()
{
    _tmxMap = nullptr;
}

bool GameLayer::loadLevel(const std::string& levelName)
{
    _currentLevelName = levelName;
    _tmxMap = ax::FastTMXTiledMap::create(_currentLevelName);
    if (!_tmxMap)
    {
        AXLOGE("Level {} failed to load.", _currentLevelName);
        return false;
    }

    if (!_tmxMap->getObjectGroup("entities") || !_tmxMap->getObjectGroup("obstacles"))
    {
        AXLOGE("Level {}. Object layers failed to load.", _currentLevelName);
        return false;
    }

    _gameplayObjects = _tmxMap->getLayer("gameplayObjects");
    if (!_gameplayObjects)
    {
        AXLOGE("Level {}. Can't find gameplayObjects", _currentLevelName);
        AX_ASSERT(false);
        return false;
    }

    addChild(_tmxMap);
    if (!loadObstacles() || !loadEntities())
    {
        return false;
    }

    // Set names to layers
    _tmxMap->setName("tmxMap");
    for (auto const& item : _tmxMap->getChildren())
    {
        if (FastTMXLayer* layer = dynamic_cast<FastTMXLayer*>(item))
        {
            layer->setName(layer->getLayerName());
        }
    }

    // Find player position for debug
    std::pair<int, int> playerPos = {0, 0};
    for (const auto& item : _spawnEntity)
    {
        if (item.node == _player)
        {
            playerPos = item.spawnPos;
            break;
        }
    }

    AXLOGI("Player position in pixels is {}, {}. Tile numbers are {}, {}", _player->getPosition().x,
           _player->getPosition().y, playerPos.first, playerPos.second);

    return true;
}

bool GameLayer::loadEntities()
{
    const auto entitiesGroup = _tmxMap->getObjectGroup("entities");
    const auto tileSize = _tmxMap->getTileSize();

    _spawnEntity.clear();
    for (const auto& entityObj : entitiesGroup->getObjects())
    {
        if (entityObj.getType() == Value::Type::MAP)
        {
            auto val = entityObj.asValueMap();

            if (val["type"].getType() != Value::Type::STRING ||
                getEntityType(val["type"].asString()) == eGameLayerType::NONE)
            {
                AXLOGE("Level {}. The entity '{}' failed to load, unsupported type.", _currentLevelName,
                       val["id"].asString());
                continue;
            }
            auto type = getEntityType(val["type"].asString());

            // Get position tile numbers on the map, counted from the left bottom
            auto tileX = static_cast<int>(val["x"].asFloat() + val["width"].asFloat() / 2);
            auto tileY = static_cast<int>(val["y"].asFloat() + val["height"].asFloat() / 2);
            auto tilePos = getTilePosition(tileSize, tileX, tileY);

            // Create entity
            sSpawnEntity entity = {.type = eGameLayerType::NONE, .node = nullptr, .spawnPos = tilePos};
            if (type == eGameLayerType::PLAYER)
            {
                if (Player* player = Player::create())
                {
                    if (_player)
                    {
                        AXLOGE("Level {}. Must be only one player", _currentLevelName);
                        AX_ASSERT(false);
                        return false;
                    }
                    _player = player;
                    entity.node = _player;
                    entity.type = eGameLayerType::PLAYER;
                }
            }
            else if (type == eGameLayerType::ENEMY)
            {
                if (Enemy* enemy = Enemy::create())
                {
                    entity.node = enemy;
                    entity.type = eGameLayerType::ENEMY;
                }
            }
            else if (type == eGameLayerType::WORLD_OBJECT)
            {
                if (WorldObject* worldObject = WorldObject::create())
                {
                    entity.node = worldObject;
                    entity.type = eGameLayerType::WORLD_OBJECT;
                    // todo subclasses: star, chest, key
                }
            }

            // Add entity to the scene, set position and store it in the list
            if (entity.type != eGameLayerType::NONE)
            {
                if (!addEntityToLayer(entity.node))
                {
                    AXLOGE("Level {}. Can't add node type {}, position {}, {}", _currentLevelName,
                           magic_enum::enum_name(entity.type), tilePos.first, tilePos.second);
                    AX_ASSERT(false);
                    return false;
                }
                entity.node->setName(getTypeName(entity.type) + " id " + val["id"].asString());
                entity.node->setPosition(val["x"].asFloat(), val["y"].asFloat());
                _spawnEntity.push_back(entity);
            }
        }
    }

    if (!_player)
    {
        AXLOGE("Level {}. No player on the scene.", _currentLevelName);
        AX_ASSERT(false);
        return false;
    }
    return true;
}

bool GameLayer::loadObstacles()
{
    auto obstaclesGroup = _tmxMap->getObjectGroup("obstacles");
    for (const auto& obstacleObj : obstaclesGroup->getObjects())
    {
        if (obstacleObj.getType() == Value::Type::MAP)
        {
            auto val = obstacleObj.asValueMap();
            auto rect = ax::Rect();
            rect.origin.x = val["x"].asFloat();
            rect.origin.y = val["y"].asFloat();
            rect.size.width = val["width"].asFloat();
            rect.size.height = val["height"].asFloat();

            if (rect.size.width <= 0.f || rect.size.height <= 0.f)
            {
                AXLOGE("Level {}. Obstacle id {} has invalid size: width {}, height {}", _currentLevelName,
                       val["id"].asString(), rect.size.width, rect.size.height);
                AX_ASSERT(false);
                return false;
            }

            _collisionList.push_back(rect);

#if DEBUG_DRAW
            if (const auto item = DrawNode::create())
            {
                _tmxMap->addChild(item, magic_enum::enum_integer(eZOrder::DEBUG_DRAWING));
                item->drawSolidRect({}, rect.size, Color32::green, 1.0f);
                item->setPosition(rect.origin);
                item->setName(getTypeName(eGameLayerType::OBSTACLE) + ": " + val["name"].asString() + " id " +
                              val["id"].asString());
            }
#endif
        }
    }

    return true;
}

bool GameLayer::addEntityToLayer(ax::Node* node) const
{
    if (!_tmxMap || !_gameplayObjects)
    {
        return false;
    }

    _gameplayObjects->addChild(node);
    return true;
}

bool GameLayer::initKeyboard()
{
    auto listener = KeyboardEventListener::create();
    if (!listener)
    {
        AXLOGE("Level {}: failed to init keyboard.", _currentLevelName);
        AX_ASSERT(false);
        return false;
    }

    listener->onKeyPressed = [this](KeyboardEvent* event) { onKeyboardInput(event->getKeyCode(), true); };

    listener->onKeyReleased = [this](KeyboardEvent* event) { onKeyboardInput(event->getKeyCode(), false); };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void GameLayer::onKeyboardInput(ax::KeyboardEvent::KeyCode keyCode, bool isPressed)
{
    const auto action = getPlayerAction(keyCode);
    if (action == ePlayerAction::NONE)
    {
        return;
    }

    if (isPressed)
    {
        _inputActions[action]++;
    }
    else
    {
        _inputActions[action]--;
        if (_inputActions[action] < 0)
        {
            AXLOGE("Level {}: action {} counter is below zero.", _currentLevelName, action);
            AX_ASSERT(false);
            _inputActions[action] = 0;
        }
    }

#if DEBUG_ACTION
    std::string data;
    for (const auto& [first, second] : _inputActions)
    {
        if (!data.empty())
            data += "; ";
        data += fmt::format("action: {} status: {}", magic_enum::enum_name(first), second);
    }
    AXLOGI("{}", data);
#endif
}
