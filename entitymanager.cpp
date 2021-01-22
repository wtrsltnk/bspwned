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

EntityManager::~EntityManager()
{
    while (!mEntities.empty())
    {
        tEntity *entity = mEntities.back();

        mEntities.pop_back();

        delete[] entity->values;
        delete entity;
    }
}

bool EntityManager::parseFromBSPEntityData(
    unsigned char *data,
    int size)
{
    Tokenizer tok(data, size);

    while (tok.nextToken() && strcmp(tok.getToken(), "{") == 0)
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
    while (tok.nextToken() && strcmp(tok.getToken(), "}") != 0)
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

    if (values.size() > 0)
    {
        tEntity *entity = new tEntity;
        entity->valueCount = values.size();
        entity->values = new tEntityValue[entity->valueCount];

        for (int i = 0; i < entity->valueCount; i++)
        {
            entity->values[i] = values[i];
            if (values[i].key == "classname")
            {
                entity->className = values[i].value;
            }
        }
        mEntities.push_back(entity);
    }
    return true;
}

int EntityManager::addEntity(
    tEntity *entity)
{
    mEntities.push_back(entity);

    return mEntities.size() - 1;
}

int EntityManager::getEntityCount() const
{
    return mEntities.size();
}

const tEntity *EntityManager::getEntity(
    int index) const
{
    if (index >= 0 && index < this->getEntityCount())
    {
        return mEntities[index];
    }

    return nullptr;
}

glm::vec3 EntityManager::getPlayerStart() const
{
    glm::vec3 position;

    for (int i = 0; i < this->getEntityCount(); i++)
    {
        tEntity *entity = mEntities[i];
        std::string entityname = entity->className;
        if (entityname != "info_player_start")
        {
            continue;
        }

        for (int j = 0; j < entity->valueCount; j++)
        {
            tEntityValue &value = entity->values[j];
            std::string valuename = value.key;
            if (valuename == "origin")
            {
                std::istringstream(value.value) >> position[0] >> position[1] >> position[2];

                return position;
            }
        }
    }
}
