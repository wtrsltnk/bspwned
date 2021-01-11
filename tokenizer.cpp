/* 
 * File:   tokenizer.cpp
 * Author: Wouter
 * 
 * Created on April 8, 2009, 11:21 PM
 */

#include "tokenizer.h"
#include <string>

using namespace std;

class Tokenizer::PIMPL
{
public:
    PIMPL(const unsigned char* data, int size) : mData(data), mDataSize(size), mCursor(0) {}
    const unsigned char* mData;
    int mDataSize;
    int mCursor;
    string mToken;
};

Tokenizer::Tokenizer(const unsigned char* data, int size)
        : pimpl(new Tokenizer::PIMPL(data, size))
{
    pimpl->mToken = "";
}

Tokenizer::~Tokenizer()
{
    delete pimpl;
}

bool isSpaceCharacter(const char c)
{
    return (c <= ' ');
}

bool Tokenizer::nextToken()
{
    pimpl->mToken = "";

    while (pimpl->mCursor <= pimpl->mDataSize && isSpaceCharacter(pimpl->mData[pimpl->mCursor]))
		pimpl->mCursor++;

    if (pimpl->mData[pimpl->mCursor] == '\"')
    {
        pimpl->mCursor++;

        while (pimpl->mCursor <= pimpl->mDataSize && pimpl->mData[pimpl->mCursor] != '\"')
        {
            pimpl->mToken += pimpl->mData[pimpl->mCursor];
            pimpl->mCursor++;
        }

        pimpl->mCursor++;
    }
    else
    {
        while (pimpl->mCursor <= pimpl->mDataSize && !isSpaceCharacter(pimpl->mData[pimpl->mCursor]))
        {
            pimpl->mToken += pimpl->mData[pimpl->mCursor];
            pimpl->mCursor++;
        }
    }

    return (pimpl->mCursor < pimpl->mDataSize);
}

const char* Tokenizer::getToken() const
{
    return pimpl->mToken.c_str();
}
