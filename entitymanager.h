/* 
 * File:   entitymanager.h
 * Author: wouter
 *
 * Created on April 17, 2009, 4:03 PM
 */

#ifndef _ENTITYMANAGER_H
#define	_ENTITYMANAGER_H

#include "types.h"

class EntityManager
{
public:
    EntityManager();
    virtual ~EntityManager();
    
    bool parseFromBSPEntityData(unsigned char* data, int size);

    int addEntity(tEntity* entity);
    int getEntityCount() const;
    const tEntity* getEntity(int index) const;

    void getPlayerStart(float position[3]) const;

private:
    class PIMPL;
    PIMPL* pimpl;

};

#endif	/* _ENTITYMANAGER_H */
