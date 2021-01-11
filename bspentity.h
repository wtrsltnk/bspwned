/* 
 * File:   bspentity.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 9:33 PM
 */

#ifndef _BSPENTITY_H
#define	_BSPENTITY_H

#include <map>
#include <string>

class BSPEntity
{
    friend class BSPReader;
public:
    BSPEntity();
    virtual ~BSPEntity();

    const char* getClassName() const;
    const char* getValue(const char* key) const;
    
private:
    std::string mClassName;
    std::map<std::string, std::string> mEntityKeys;
    
};

#endif	/* _BSPENTITY_H */

