#define {rpc_module_upper}_{rpc_function_upper} {rpc_id}

{rpc_structs}

{C_response_type} {rpc_module}_{rpc_function}({rpc_args});
void rpc_response_{rpc_function}(pid_t pid, {C_response_type} response);
{rpc_module}_req_{rpc_function}_t rpc_request_{rpc_function}_unpack(ipc_t *message);
