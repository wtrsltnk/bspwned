/* 
 * File:   stringcompare.h
 * Author: wouter
 *
 * Created on April 22, 2009, 4:48 PM
 */

#ifndef _STRINGCOMPARE_H
#define	_STRINGCOMPARE_H

// THIS IS AN UGLY HACK, MAKE A NICE CLASS OR SOME THING PLZ!
typedef struct str_s { const char* str; } str;

bool operator == (str& l, str& r);
bool operator == (str& l, const char* r);
str& make_str(const char* s);

#endif	/* _STRINGCOMPARE_H */

