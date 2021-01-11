#include "stringcompare.h"
#include <string.h>
#include <ctype.h>

bool operator == (str& l, str& r)
{
    int i = 0;
    while (l.str[i] != '\0' && r.str[i] != '\0')
    {
        if (tolower(l.str[i]) != tolower(r.str[i]))
            return false;
        i++;
    }
    return (tolower(l.str[i]) == tolower(r.str[i]));
}

bool operator == (str& l, const char* r)
{
    int i = 0;
    while (l.str[i] != '\0' && r[i] != '\0')
    {
        if (tolower(l.str[i]) != tolower(r[i]))
            return false;
        i++;
    }
    return (tolower(l.str[i]) == tolower(r[i]));
}

str& make_str(const char* s)
{
    static str r;
    r.str = s;
    return r;
}
