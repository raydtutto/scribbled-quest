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
        AXLOGE("The level path '{}' failed to load.", levelName);
        return false;
    }

    if (!_tmxMap->getObjectGroup("entities") || !_tmxMap->getObjectGroup("obstacles"))
    {
        AXLOGE("Object layers from the '{}' failed to load.", levelName);
        return false;
    }

    addChild(_tmxMap);
    loadEntities();
    loadObstacles();

    return true;
}

std::pair<int, int> getTilePosition(const Vec2 tileSize, const int tileX, const int tileY)
{
    return {ceil(tileX / tileSize.x) - 1, ceil(tileY / tileSize.y) - 1};
}

void GameLayer::loadEntities()
{
    auto entitiesGroup = _tmxMap->getObjectGroup("entities");
    auto tileSize = _tmxMap->getTileSize();

    _spawnEntity.clear();
    for (const auto& entityObj : entitiesGroup->getObjects())
    {
        if (entityObj.getType() == Value::Type::MAP)
        {
            auto val = entityObj.asValueMap();

            if (val["type"].getType() != Value::Type::STRING ||
                getEntityType(val["type"].asString()) == eGameLayerType::NONE)
            {
                AXLOGE("The entity '{}' failed to load, unsupported type.", val["id"].asString());
                continue;
            }
            std::string id = val["id"].asString();
            auto type = getEntityType(val["type"].asString());

            // Get position tile numbers on the map, counted from the bottom
            auto tileX = static_cast<int>(val["x"].asFloat() + val["width"].asFloat() / 2);
            auto tileY = static_cast<int>(val["y"].asFloat() + val["height"].asFloat() / 2);
            auto tilePos = getTilePosition(tileSize, tileX, tileY);
            AXLOGI("id: {}, type: {}, position: [{}, {}]", id, getTypeName(type), tilePos.first, tilePos.second);

            if (type == eGameLayerType::PLAYER)
            {
                if (Player* player = Player::create())
                {
                    addChild(player);
                    player->setPosition(val["x"].asFloat(), val["y"].asFloat());

                    _spawnEntity.push_back(sSpawnEntity(eGameLayerType::PLAYER, player, tilePos));
                }
            }

            if (type == eGameLayerType::ENEMY)
            {
                Enemy* enemy = Enemy::create();
                if (enemy)
                {
                    addChild(enemy);
                    AXLOGD("Enemy created.");

                    auto x = val["x"].asFloat();
                    auto y = val["y"].asFloat();

                    enemy->setPosition(val["x"].asFloat(), val["y"].asFloat());
                }
            }
        }
    }
}
void GameLayer::loadObstacles()
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

            _collisionList.push_back(rect);

#ifdef DEBUG_DRAW
            auto item = DrawNode::create();
            if (item)
            {
                addChild(item);
                item->drawSolidRect({}, rect.size, Color32::green, 1.0f);
                item->setPosition(rect.origin);
            }
#endif
        }
    }
}
