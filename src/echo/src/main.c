#include <unistd.h>
#include <stdio.h>
#include <json.h>
#include <assert.h>

#include <protocol/inc/echo.h>

#define ECHO_MESSAGE_SIZE 8192

static void client(void)
{
    char *message = strdup("Hello, World !");
    printf("Client is sending message...\n");
    echo_send_message(message);
}

static void server(void)
{
    char *message = (char *) ipc_receive_sync();
    printf("Server received message: %s\n", message);
}

int _start(void)
{
    printf("Hello from echo !\n");
    pid_t pid = getpid();

    if (pid == 1)
    {
        client();
    }
    else  
    {
        server();
    }

    for (;;);
    __builtin_unreachable();
}
