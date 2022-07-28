#include <stdlib.h>

long atol(const char *nptr)
{
    long ret = 0;
    int sign = 1;

    if (*nptr == '-')
    {
        sign = -1;
        nptr++;
    }

    while (*nptr)
    {
        ret = ret * 10 + (*nptr - '0');
        nptr++;
    }

    return ret * sign;
}
