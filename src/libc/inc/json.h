#ifndef LIBC_INC_JSON_H
#define LIBC_INC_JSON_H

#include <stdbool.h>

#include "vec.h"
#include "map.h"
#include "reader.h"

typedef enum 
{
    JSON_ERROR,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
    JSON_NULL,
    JSON_PTR,
} json_type;

typedef struct json json_t;

typedef vec(json_t) json_vec_t;
typedef map_t(json_t) json_obj_t;

struct json
{
    json_type type;

    union 
    {
        json_vec_t _array;
        json_obj_t _object;
        char *_string;
        int64_t _number;
        bool _bool;
        void *_ptr;
    };
};

json_t json_object(void);
json_t json_parse(char const *str);
json_t json_parse_reader(reader_t *r);
json_t json_get(json_t const j, char const *key);
json_t json_string(char const *s);
json_t json_number(int64_t i);
json_t json_bool(bool b);
json_t json_ptr(void *p);
char *json_dump(json_t j);
char *json_dump_object(json_t const j, vec_char_t builder);
void json_push(json_t *j, char const *key, json_t value);
void json_free(json_t *j);

#endif /* !LIBC_INC_JSON_H */
