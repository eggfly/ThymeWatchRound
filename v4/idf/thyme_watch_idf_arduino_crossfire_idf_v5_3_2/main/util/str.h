#pragma once
#include <string.h>
#include <ctype.h>
#include <climits>

int strncmpci(const char *str1, const char *str2, size_t num)
{
    int ret_code = 0;
    size_t chars_compared = 0;

    if (!str1 || !str2)
    {
        ret_code = INT_MIN;
        return ret_code;
    }

    while ((chars_compared < num) && (*str1 || *str2))
    {
        ret_code = tolower((int)(*str1)) - tolower((int)(*str2));
        if (ret_code != 0)
        {
            break;
        }
        chars_compared++;
        str1++;
        str2++;
    }

    return ret_code;
}

// use inline
inline bool startsWithIgnoreCase(const char *pre, const char *str)
{
    return strncmpci(pre, str, strlen(pre)) == 0;
}

inline bool endsWithIgnoreCase(const char *base, const char *str)
{
    int blen = strlen(base);
    int slen = strlen(str);
    return (blen >= slen) && (0 == strncmpci(base + blen - slen, str, strlen(str)));
}
