/* 
 * File:   error.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 10:55 PM
 */

#ifndef _ERROR_H
#define	_ERROR_H

class Error
{
public:
    Error();
    virtual ~Error();

    const char* getLastError();

protected:
    void setError(const char* message);

private:
    char* mLastError;

};

#endif	/* _ERROR_H */

