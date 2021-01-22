/* 
 * File:   tokenizer.cpp
 * Author: Wouter
 * 
 * Created on April 8, 2009, 11:21 PM
 */

#include "tokenizer.h"

Tokenizer::Tokenizer(
    std::unique_ptr<unsigned char> &data,
    int size)
    : _data(data),
      _dataSize(size),
      _cursor(0),
      _token("")
{}

Tokenizer::~Tokenizer() = default;

bool isSpaceCharacter(
    const char c)
{
    return (c <= ' ');
}

bool Tokenizer::nextToken()
{
    _token = "";

    while (_cursor <= _dataSize && isSpaceCharacter(_data.get()[_cursor]))
    {
        _cursor++;
    }

    if (_data.get()[_cursor] == '\"')
    {
        _cursor++;

        while (_cursor <= _dataSize && _data.get()[_cursor] != '\"')
        {
            _token += _data.get()[_cursor];
            _cursor++;
        }

        _cursor++;
    }
    else
    {
        while (_cursor <= _dataSize && !isSpaceCharacter(_data.get()[_cursor]))
        {
            _token += _data.get()[_cursor];
            _cursor++;
        }
    }

    return (_cursor < _dataSize);
}

const std::string &Tokenizer::getToken() const
{
    return _token;
}
