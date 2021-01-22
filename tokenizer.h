/* 
 * File:   tokenizer.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 11:21 PM
 */

#ifndef _TOKENIZER_H
#define _TOKENIZER_H

#include <memory>
#include <string>

class Tokenizer
{
public:
    Tokenizer(
        std::unique_ptr<unsigned char> &data,
        int size);

    virtual ~Tokenizer();

    bool nextToken();

    const std::string &getToken() const;

private:
    std::unique_ptr<unsigned char> &_data;
    int _dataSize;
    int _cursor;
    std::string _token;
};

#endif /* _TOKENIZER_H */
