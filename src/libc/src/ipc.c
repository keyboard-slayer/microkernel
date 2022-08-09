#include "../inc/ipc.h"
#include <json.h>

int rpc_get_function(ipc_t *ipc)
{
    json_t req = json_parse((char const *) ipc->payload);

    if (req.type != JSON_OBJECT)
    {
        return -1;
    }

    json_t rpc_function = json_get(req, "rpc_function");

    if (rpc_function.type != JSON_NUMBER)
    {
        return -1;
    }

    return rpc_function._number;
}
