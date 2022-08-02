{C_response_type} {rpc_module}_{rpc_function}({rpc_args})
{{
    json_t {rpc_module}_{rpc_function}_req = json_object();
    json_push(&{rpc_module}_{rpc_function}_req, "rpc_function", json_string("{rpc_function}"));

    json_t rpc_args = json_object();
    {rpc_args_push}
    json_push(&{rpc_module}_{rpc_function}_req, "rpc_args", rpc_args);

    ipc_t *message = calloc(sizeof(ipc_t), 1);
    message->identifier = {rpc_identifier};
    message->payload = (uintptr_t) json_dump({rpc_module}_{rpc_function}_req);
    message->payload_require_free = true;

    ipc_send(message);
    message = (ipc_t *) ipc_receive_sync();

    json_t resp = json_parse((char const *) message->payload);
    json_t success = json_get(resp, "rpc_success");
    assert(success._bool);

    json_t rpc_resp = json_get(resp, "rpc_response");
    assert(rpc_resp.type == JSON_{rpc_response_type});
    {rpc_return}

    if (message->payload_require_free)
    {{
        free((void *) message->payload);
    }}

    free(message);
    json_free(&{rpc_module}_{rpc_function}_req);
    json_free(&rpc_args);
    {rpc_ret}
}}

void rpc_response_{rpc_function}(pid_t pid, {C_response_type} response)
{{
    json_t {rpc_module}_{rpc_function}_resp = json_object();
    json_push(&{rpc_module}_{rpc_function}_resp, "rpc_function", json_number({rpc_function_UPPER}));
    json_push(&{rpc_module}_{rpc_function}_resp, "rpc_response", json_{JSON_response_type}(response));
    json_push(&{rpc_module}_{rpc_function}_resp, "rpc_success", json_bool(true));

    ipc_t *ipc = calloc(sizeof(ipc), 1);
    ipc->dst = pid;
    ipc->payload = (uintptr_t) json_dump({rpc_module}_{rpc_function}_resp);
    ipc->payload_require_free = true;
    
    ipc_send(ipc);
    json_free(&{rpc_module}_{rpc_function}_resp);
}}

{rpc_module}_req_{rpc_function}_t rpc_request_{rpc_function}_unpack(ipc_t *message)
{{
    json_t request = json_parse((const char *) message->payload);
    json_t rpc_args = json_get(request, "rpc_args");
    {rpc_module}_req_{rpc_function}_t ret = {{
        {rpc_args_unpack}
    }};

    if (message->payload_require_free)
    {{
        free((void *) message->payload);
    }}

    free(message);

    return ret;
}}
