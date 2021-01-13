/* 
 * File:   entitymanager.cpp
 * Author: wouter
 * 
 * Created on April 17, 2009, 4:03 PM
 */

#include "entitymanager.h"

#include "tokenizer.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

class EntityManager::PIMPL
{
public:
    vector<tEntity *> mEntities;

    bool parseEntity(Tokenizer &tok);
};

EntityManager::EntityManager()
{
    this->pimpl = new EntityManager::PIMPL();
}

EntityManager::~EntityManager()
{
    while (!pimpl->mEntities.empty())
    {
        tEntity *entity = pimpl->mEntities.back();
        for (int i = 0; i < entity->valueCount; i++)
        {
            tEntityValue *value = &entity->values[i];
            delete[] value->key;
            delete[] value->value;
        }
        delete[] entity->values;
        delete entity;
        pimpl->mEntities.pop_back();
    }

    delete pimpl;
}

bool EntityManager::parseFromBSPEntityData(
    unsigned char *data,
    int size)
{
    Tokenizer tok(data, size);

    while (tok.nextToken() && strcmp(tok.getToken(), "{") == 0)
    {
        if (!pimpl->parseEntity(tok))
            return false;
    }
    return true;
}

bool EntityManager::PIMPL::parseEntity(
    Tokenizer &tok)
{
    vector<tEntityValue> values;
    while (tok.nextToken() && strcmp(tok.getToken(), "}") != 0)
    {
        string key = tok.getToken();
        if (!tok.nextToken())
            break;

        string value = tok.getToken();
        if (value == "}")
            break;

        tEntityValue eValue;
        eValue.key = new char[key.size() + 1];
        strcpy(eValue.key, key.c_str());
        eValue.value = new char[value.size() + 1];
        strcpy(eValue.value, value.c_str());

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
            if (strcmp(values[i].key, "classname") == 0)
            {
                strcpy(entity->className, values[i].value);
            }
        }
        mEntities.push_back(entity);
    }
    return true;
}

int EntityManager::addEntity(
    tEntity *entity)
{
    pimpl->mEntities.push_back(entity);

    return pimpl->mEntities.size() - 1;
}

int EntityManager::getEntityCount() const
{
    return pimpl->mEntities.size();
}

const tEntity *EntityManager::getEntity(int index) const
{
    if (index >= 0 && index < this->getEntityCount())
        return pimpl->mEntities[index];

    return NULL;
}

glm::vec3 EntityManager::getPlayerStart() const
{
    glm::vec3 position;

    for (int i = 0; i < this->getEntityCount(); i++)
    {
        tEntity *entity = pimpl->mEntities[i];
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
