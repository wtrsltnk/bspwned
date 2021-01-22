/* 
 * File:   entitymanager.cpp
 * Author: wouter
 * 
 * Created on April 17, 2009, 4:03 PM
 */

#include "entitymanager.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

EntityManager::EntityManager() = default;

EntityManager::~EntityManager() = default;

bool EntityManager::parseFromBSPEntityData(
    std::unique_ptr<unsigned char> &data,
    int size)
{
    Tokenizer tok(data, size);

    while (tok.nextToken() && tok.getToken() == "{")
    {
        if (!parseEntity(tok))
        {
            return false;
        }
    }

    return true;
}

bool EntityManager::parseEntity(
    Tokenizer &tok)
{
    std::vector<tEntityValue> values;

    while (tok.nextToken() && tok.getToken() != "}")
    {
        std::string key = tok.getToken();
        if (!tok.nextToken())
        {
            break;
        }

        std::string value = tok.getToken();
        if (value == "}")
        {
            break;
        }

        tEntityValue eValue;
        eValue.key = key;
        eValue.value = value;

        values.push_back(eValue);
    }

    if (values.size() == 0)
    {
        return false;
    }

    tEntity entity;

    entity.values = values;

    for (auto &value : values)
    {
        if (value.key == "classname")
        {
            entity.className = value.value;
        }
    }

    mEntities.push_back(entity);

    return true;
}

int EntityManager::addEntity(
    const tEntity &entity)
{
    mEntities.push_back(entity);

    return static_cast<int>(mEntities.size() - 1);
}

const std::vector<tEntity> &EntityManager::getEntities() const
{
    return mEntities;
}

glm::vec3 EntityManager::getPlayerStart() const
{
    glm::vec3 position;

    for (auto &entity : mEntities)
    {
        std::string entityname = entity.className;
        if (entityname != "info_player_start")
        {
            continue;
        }

        for (auto &value : entity.values)
        {
            std::string valuename = value.key;
            if (valuename == "origin")
            {
                std::istringstream(value.value) >> position[0] >> position[1] >> position[2];

                return position;
            }
        }
    }

    return position;
}
