#include "GameLayer.h"

#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/2d/DrawNode.h"
#include "gameplay/Obstacle.h"
#include "gameplay/Player.h"
#include "gameplay/Enemy.h"

// Drawing level collisions
#define DEBUG_DRAW 1

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

std::pair<int, int> getTilePosition(const Vec2 tileSize, const int tileX, const int tileY)
{
    return {ceil(tileX / tileSize.x) - 1, ceil(tileY / tileSize.y) - 1};
}
}  // namespace

GameLayer* GameLayer::create(const std::string& levelName)
{
    GameLayer* pRet = new GameLayer();
    if (pRet->init() && pRet->loadLevel(levelName))
    {
        pRet->autorelease();
        return pRet;
    }

    delete pRet;
    pRet = nullptr;
    return nullptr;
}

GameLayer::~GameLayer()
{
    _tmxMap = nullptr;
}

bool GameLayer::loadLevel(const std::string& levelName)
{
    _tmxMap = ax::FastTMXTiledMap::create(levelName);
    if (!_tmxMap)
    {
        AXLOGE("Level {} failed to load.", levelName);
        return false;
    }

    if (!_tmxMap->getObjectGroup("entities") || !_tmxMap->getObjectGroup("obstacles"))
    {
        AXLOGE("Level {}. Object layers failed to load.", levelName);
        return false;
    }

    addChild(_tmxMap);
    if (!loadEntities(levelName) || !loadObstacles(levelName))
    {
        return false;
    }

    return true;
}

bool GameLayer::loadEntities(const std::string& levelName)
{
    const auto entitiesGroup = _tmxMap->getObjectGroup("entities");
    const auto tileSize = _tmxMap->getTileSize();
    std::map<std::string, int> createdEntitiesCounter;

    _spawnEntity.clear();
    for (const auto& entityObj : entitiesGroup->getObjects())
    {
        if (entityObj.getType() == Value::Type::MAP)
        {
            auto val = entityObj.asValueMap();

            if (val["type"].getType() != Value::Type::STRING ||
                getEntityType(val["type"].asString()) == eGameLayerType::NONE)
            {
                AXLOGE("Level {}. The entity '{}' failed to load, unsupported type.", levelName, val["id"].asString());
                continue;
            }
            std::string id = val["id"].asString();
            auto type = getEntityType(val["type"].asString());

            // Get position tile numbers on the map, counted from the left bottom
            auto tileX = static_cast<int>(val["x"].asFloat() + val["width"].asFloat() / 2);
            auto tileY = static_cast<int>(val["y"].asFloat() + val["height"].asFloat() / 2);
            auto tilePos = getTilePosition(tileSize, tileX, tileY);
            AXLOGI("id: {}, type: {}, position: [{}, {}]", id, getTypeName(type), tilePos.first, tilePos.second);

            // Create entity
            sSpawnEntity entity = {.type = eGameLayerType::NONE, .node = Node::create(), .spawnPos = tilePos};
            if (type == eGameLayerType::PLAYER)
            {

                if (Player* player = Player::create())
                {
                    entity.node = player;
                    entity.type = eGameLayerType::PLAYER;
                    createdEntitiesCounter[val["type"].asString()]++;
                }
            }
            else if (type == eGameLayerType::ENEMY)
            {
                if (Enemy* enemy = Enemy::create())
                {
                    entity.node = enemy;
                    entity.type = eGameLayerType::ENEMY;
                    createdEntitiesCounter[val["type"].asString()]++;
                }
            }
            else if (type == eGameLayerType::WORLD_OBJECT)
            {
                // todo add world_object
                entity.type = eGameLayerType::NONE;
            }

            // Add entity to the scene, set position and store it in the list
            if (entity.type != eGameLayerType::NONE)
            {
                addChild(entity.node);
                entity.node->setPosition(val["x"].asFloat(), val["y"].asFloat());
                _spawnEntity.push_back(entity);
            }
        }
    }

    // Validation
    if (createdEntitiesCounter["player"] != 1)
    {
        AXLOGE("Level {}. Player count is {}, must be only one player", levelName, createdEntitiesCounter["player"]);
        AX_ASSERT(false);
        return false;
    }

    return true;
}

bool GameLayer::loadObstacles(const std::string& levelName)
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
                AXLOGE("Level {}. Obstacle id {} has invalid size: width {}, height {}", levelName,
                       val["id"].asString(), rect.size.width, rect.size.height);
                AX_ASSERT(false);
                return false;
            }

            _collisionList.push_back(rect);

#if DEBUG_DRAW
            if (const auto item = DrawNode::create())
            {
                addChild(item);
                item->drawSolidRect({}, rect.size, Color32::green, 1.0f);
                item->setPosition(rect.origin);
            }
#endif
        }
    }

    return true;
}
