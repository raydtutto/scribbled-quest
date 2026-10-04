#include "GameLayer.h"

#include "axmol/2d/FastTMXTiledMap.h"

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
    case eGameLayerType::CHEST:
        return "chest";
        break;
    case eGameLayerType::STAR:
        return "star";
        break;
    case eGameLayerType::KEY:
        return "key";
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
    if (typeName == "chest")
        return eGameLayerType::CHEST;
    if (typeName == "star")
        return eGameLayerType::STAR;
    if (typeName == "key")
        return eGameLayerType::KEY;

    return eGameLayerType::NONE;
}
}


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

    for (const auto& entity : entitiesGroup->getObjects())
    {
        if (entity.getType() == Value::Type::MAP)
        {
            auto val = entity.asValueMap();

            if (val["type"].getType() != Value::Type::STRING || getEntityType(val["type"].asString()) == eGameLayerType::NONE)
            {
                AXLOGE("The entity '{}' failed to load, unsupported type.", val["id"].asString());
                continue;
            }
            std::string id = val["id"].asString();
            auto type = getEntityType(val["type"].asString());

            // Get tile position on the map, counted from the bottom
            auto tileX = static_cast<int>(val["x"].asFloat() + val["width"].asFloat() / 2);
            auto tileY = static_cast<int>(val["y"].asFloat() + val["height"].asFloat() / 2);
            auto tilePos = getTilePosition(tileSize, tileX, tileY);
            AXLOGI("id: {}, type: {}, position: [{}, {}]", id, getTypeName(type), tilePos.first, tilePos.second);

            if (type == eGameLayerType::PLAYER)
            {
                // spawn player
            }
        }
    }
}
