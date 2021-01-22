/* 
 * File:   entitymanager.h
 * Author: wouter
 *
 * Created on April 17, 2009, 4:03 PM
 */

#ifndef _ENTITYMANAGER_H
#define _ENTITYMANAGER_H

#include "tokenizer.h"
#include "types.h"

#include <glm/glm.hpp>
#include <vector>

class EntityManager
{
public:
    EntityManager();

    virtual ~EntityManager();

    bool parseFromBSPEntityData(
        unsigned char *data,
        int size);

    int addEntity(
        tEntity *entity);

    int getEntityCount() const;

    const tEntity *getEntity(
        int index) const;

    glm::vec3 getPlayerStart() const;

private:
    std::vector<tEntity *> mEntities;

    bool parseEntity(
        Tokenizer &tok);
};

#endif /* _ENTITYMANAGER_H */
