#include <unistd.h>
#include <stdio.h>
#include <json.h>
#include <assert.h>
#include <ipc.h>

#include <idl/inc/echo.h>

#define ECHO_MESSAGE_SIZE 8192

static void client(void)
{
    char *message = strdup("Hello, World !");
    printf("Client is sending message...\n");
    char *resp = echo_send_message(message);
    printf("Client received %s\n", resp);
}

static void server(void)
{
    printf("Waiting for IPC ...\n");
    ipc_t *ipc = (ipc_t *) ipc_receive_sync();

    echo_req_send_message_t message = rpc_request_send_message_unpack(ipc);
    printf("Server received message: %s\n", message.m);
    rpc_response_send_message(ipc->src, "Ack!");
}

int _start(void)
{
    pid_t pid = getpid();

    if (pid == 1)
    {
        client();
    }
    else  
    {
        assert(ipc_set_identifier(ECHO_IDENT) == 0);
        server();
    }

    for (;;);
    __builtin_unreachable();
}
