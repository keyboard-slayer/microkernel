json_t json_{rpc_struct}({rpc_struct} data)
{{
    json_t ret = json_object();

{rpc_struct_push}

    return ret;
}}