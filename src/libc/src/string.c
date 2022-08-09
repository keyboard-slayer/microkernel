#include <stdlib.h>

#include "../inc/string.h"

size_t strlen(char const *s)
{
    size_t len = 0;
    while (*s++)
    {
        len++;
    }
    return len;
}

int memcmp(void const *s1, void const *s2, size_t n)
{
    unsigned char const *p1 = s1;
    unsigned char const *p2 = s2;
    while (n--)
    {
        if (*p1 != *p2)
        {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    if (src == NULL)
    {
        dest = NULL;
        return NULL;
    }

    size_t i;
    char *cdest = (char *) dest;
    char *csrc = (char *) src;

    for (i = 0; i < n; i++)
    {
        cdest[i] = csrc[i];
    }

    return (void *) cdest;
}


char *strrchr(char const *s, int c)
{
    char *last = NULL;

    while (*s)
    {
        if (*s == c)
        {
            last = (char *)s;
        }
        s++;
    }

    return last;
}

char *strndup(char const *s, size_t n)
{
    size_t len = strlen(s);
    if (n < len)
    {
        len = n;
    }

    char *dup = malloc(len + 1);

    if (dup)
    {
        memcpy(dup, s, len);
        dup[len] = '\0';
    }

    return dup;
}

char *strdup(char const *s)
{
    return strndup(s, strlen(s));
}

int strcmp(char const *s1, char const *s2)
{
    while (*s1 && *s2)
    {
        if (*s1 != *s2)
        {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

void *memset(void *s, int c, size_t n)
{
    char *p = s;

    while (n--)
    {
        *p++ = c;
    }

    return s;
}

char *strtok(char *restrict str, const char *restrict delim)
{
    static char *p;

    if (!p && !str)
    {
        return NULL;
    }

    if (!str)
    {
        str = p;
    }
    else
    {
        p = str;
    }

    for (;;)
    {
        for (size_t i = 0; i < strlen(delim); i++)
        {
            if (*str == delim[i])
            {
                str++;
                break;
            }
        }

        if (*str == '\0')
        {
            return NULL;
        }

        break;
    }

    char *ret = str;
    for (;;)
    {
        if (*str == '\0')
        {
            p = str;
            return ret;
        }

        for (size_t i = 0; i < strlen(delim); i++)
        {
            if (*str == delim[i])
            {
                *str = '\0';
                p = str + 1;
                return ret;
            }
        }

        str++;
    }
}
