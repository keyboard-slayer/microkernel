#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "../inc/json.h"
#include "../inc/reader.h"
#include "../inc/vec.h"

static json_t json_parse_string(reader_t *r)
{
    vec_char_t string_builder;
    vec_init(&string_builder);

    while (!reader_is_end(r) && reader_next(r) != '"')
    {
        vec_push(&string_builder, reader_peek(r));
    }

    reader_next(r);

    char *ret = strndup(string_builder.data, string_builder.length);

    vec_free(&string_builder);

    return (json_t) {
        .type = JSON_STRING,
        ._string = ret
    };
}

static json_t json_parse_array(reader_t *r)
{
    json_vec_t array_builder;
    vec_init(&array_builder);

    reader_next(r);
    reader_skip_space(r);
    
    while (!reader_is_end(r) && reader_peek(r) != ']')
    {
        vec_push(&array_builder, json_parse_reader(r));
        reader_skip_space(r);
        
        if (reader_peek(r) == ',')
        {
            reader_next(r);
            reader_skip_space(r);
        }
    }
    
    reader_next(r);
    reader_skip_space(r);
    
    json_t ret = (json_t) {
        .type = JSON_ARRAY,
        ._array = array_builder
    };
    
    return ret;
}

static json_t json_parse_object(reader_t *r)
{
    json_obj_t object_builder;
    map_init(&object_builder);
    
    reader_next(r);
    reader_skip_space(r);
    
    while (!reader_is_end(r) && reader_peek(r) != '}')
    {
        json_t key = json_parse_reader(r);

        reader_skip_space(r);
        
        if (reader_peek(r) != ':')
        {
            return (json_t) {
                .type = JSON_ERROR,
                ._string = strdup("Expected ':'")
            };
        }
        
        reader_next(r);
        reader_skip_space(r);
        
        json_t value = json_parse_reader(r);
        reader_skip_space(r);

        if (key.type != JSON_STRING)
        {
            return (json_t) {
                .type = JSON_ERROR,
                ._string = strdup("Expected string key")
            };
        }
        
        map_set(&object_builder, key._string, value);
        free(key._string);
        
        if (reader_peek(r) == ',')
        {
            reader_next(r);
            reader_skip_space(r);
        }
    }
    
    reader_next(r);
    reader_skip_space(r);

    json_t ret = (json_t) {
        .type = JSON_OBJECT,
        ._object = object_builder
    };
    
    return ret;
}

static json_t json_parse_number(reader_t *r)
{
    vec_char_t number_builder;
    vec_init(&number_builder);
    
    while (!reader_is_end(r) && (isdigit(reader_peek(r))))
    {
        vec_push(&number_builder, reader_peek(r));
        reader_next(r);
    }
    
    char *tmp = strndup(number_builder.data, number_builder.length);

    long ret = atol(tmp);

    free(tmp);
    vec_free(&number_builder);

    return (json_t) {
        .type = JSON_NUMBER,
        ._number = ret
    };
}

json_t json_get(json_t json, char const *key)
{
    if (json.type != JSON_OBJECT)
    {
        return (json_t) {
            .type = JSON_ERROR,
            ._string = strdup("json_get: json is not an object")
        };
    }

    json_t *value = map_get(&json._object, key);

    if (value == NULL)
    {
        return (json_t) {
            .type = JSON_ERROR,
            ._string = strdup("json_get: key not found")
        };
    }

    return *value;
}

json_t json_parse_reader(reader_t *r)
{
    reader_skip_space(r);

    if (reader_peek(r) == '"')
    {
        return json_parse_string(r);
    }
    else if (reader_peek(r) == '-' || isdigit(reader_peek(r)))
    {
        return json_parse_number(r);
    }
    else if(reader_skip_word(r, "true"))
    {
        return (json_t) {
            .type = JSON_BOOL,
            ._bool = true
        };
    }
    else if(reader_skip_word(r, "false"))
    {
        return (json_t) {
            .type = JSON_BOOL,
            ._bool = false
        };
    }
    else if(reader_skip_word(r, "null"))
    {
        return (json_t) {
            .type = JSON_NULL
        };
    }
    else if(reader_peek(r) == '[')
    {
        return json_parse_array(r);
    }
    else if(reader_peek(r) == '{')
    {
        return json_parse_object(r);
    }

    return (json_t) {
        .type = JSON_ERROR,
        ._string = strdup("json_parse_reader: invalid json")
    };
}

json_t json_parse(char const *str)
{
    reader_t r = reader_create(str, strlen(str));
    return json_parse_reader(&r);
}

void json_free(json_t *j)
{
    if (j->type == JSON_STRING || j->type == JSON_ERROR)
    {
        free(j->_string);
    }
    else if (j->type == JSON_ARRAY)
    {
        json_vec_t *vec = &j->_array;
        for (size_t i = 0; i < vec->length; i++)
        {
            json_free(&vec->data[i]);
        }
        free(vec->data);
    }
    else if(j->type == JSON_OBJECT)
    {
        const char *key;
        map_iter_t iter = map_iter(&j->_object);

        while ((key = map_next(&j->_object, &iter)))
        {
            json_free((json_t *)map_get(&j->_object, key));
        }

        map_deinit(&j->_object);
    }
}

json_t json_object(void)
{
    json_obj_t object_builder;
    map_init(&object_builder);
    
    json_t ret = (json_t) {
        .type = JSON_OBJECT,
        ._object = object_builder
    };
    
    return ret;
}

void json_push(json_t *j, char const *key, json_t value)
{
    if (j->type != JSON_OBJECT)
    {
        return;
    }

    map_set(&j->_object, key, value);
}

json_t json_string(char const *s)
{
    json_t ret = (json_t) {
        .type = JSON_STRING,
        ._string = strdup(s)
    };
    
    return ret;
}

json_t json_number(long i)
{
    json_t ret = (json_t) {
        .type = JSON_NUMBER,
        ._number = i
    };
    
    return ret;
}

json_t json_bool(bool b)
{
    json_t ret = (json_t) {
        .type = JSON_BOOL,
        ._bool = b
    };
    
    return ret;
}

char *json_dump(json_t j)
{
    vec_char_t builder;
    vec_init(&builder);

    return json_dump_object(j, builder);
}

char *json_dump_object(json_t j, vec_char_t builder)
{
    switch(j.type)
    {
        case JSON_STRING:
        {
            vec_push(&builder, '"');

            for (size_t i = 0; i < strlen(j._string); i++)
            {
                vec_push(&builder, j._string[i]);
            }

            vec_push(&builder, '"');
            break;
        }
        case JSON_NUMBER:
        {
            char tmp[32];
            sprintf(tmp, "%ld", j._number);

            for (size_t i = 0; i < strlen(tmp); i++)
            {
                vec_push(&builder, tmp[i]);
            }

            break;
        }
        case JSON_BOOL:
        {
            if (j._bool)
            {
                vec_push(&builder, 't');
                vec_push(&builder, 'r');
                vec_push(&builder, 'u');
                vec_push(&builder, 'e');
            }
            else
            {
                vec_push(&builder, 'f');
                vec_push(&builder, 'a');
                vec_push(&builder, 'l');
                vec_push(&builder, 's');
                vec_push(&builder, 'e');
            }
            break;
        }
        case JSON_NULL:
        {
            vec_push(&builder, 'n');
            vec_push(&builder, 'u');
            vec_push(&builder, 'l');
            vec_push(&builder, 'l');
            break;
        }
        case JSON_ARRAY:
        {
            vec_push(&builder, '[');

            for (size_t i = 0; i < j._array.length; i++)
            {
                char *obj = json_dump(j._array.data[i]);

                for (size_t j = 0; j < strlen(obj); j++)
                {
                    vec_push(&builder, obj[j]);
                }

                if (i != j._array.length - 1)
                {
                    vec_push(&builder, ',');
                }
                
                free(obj);
            }
            vec_push(&builder, ']');
            break;
        }
        case JSON_OBJECT:
        {
            vec_push(&builder, '{');
            const char *key;
            map_iter_t iter = map_iter(&j._object);
            while ((key = map_next(&j._object, &iter)))
            {
                vec_push(&builder, '"');
                for (size_t i = 0; i < strlen(key); i++)
                {
                    vec_push(&builder, key[i]);
                }
                vec_push(&builder, '"');
                vec_push(&builder, ':');

                char *value = json_dump(*(json_t *)map_get(&j._object, key));
                
                for (size_t i = 0; i < strlen(value); i++)
                {
                    vec_push(&builder, value[i]);
                }

                vec_push(&builder, ',');
                free(value);
            }
            (void) vec_pop(&builder);
            vec_push(&builder, '}');
            break;
        }
        default:
        {
            vec_push(&builder, 'e');
            vec_push(&builder, 'r');
            vec_push(&builder, 'r');
            vec_push(&builder, 'o');
            vec_push(&builder, 'r');
            break;
        }
    }

    vec_push(&builder, '\0');
    char *ret = strdup(builder.data);
    vec_free(&builder);
    return ret;
}
