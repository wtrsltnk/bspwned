/* 
 * File:   tokenizer.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 11:21 PM
 */

#ifndef _TOKENIZER_H
#define	_TOKENIZER_H

class Tokenizer
{
public:
    Tokenizer(const unsigned char* data, int size);
    virtual ~Tokenizer();

    bool nextToken();
    const char* getToken() const;
    
private:
    class PIMPL;
    PIMPL* pimpl;

};

#endif	/* _TOKENIZER_H */

