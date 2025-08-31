#include <stdio.h>
#include <WinSock2.h>
#include <stdbool.h>

#pragma comment(lib, "ws2_32.lib")
#define BUFFER_SIZE 512
struct CLIENT_INFO
{
    SOCKET client_socket;
    struct sockaddr_in client_addr;
};

bool init_winsock();

int main(void)
{
    // Initialized winsock
    if (!init_winsock())
    {
        printf("Error initializing winsock.\n");
        return 1;
    }

    // create socket
    SOCKET server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        printf("Invalid socket: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Socket created.\n");

    // bind socket to an address
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7878);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to the address & port
    int bind_status = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bind_status == SOCKET_ERROR)
    {
        printf("Unable to bind socket.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Socket binded.\n");

    // put the socket in listening state so that it can wait for clients to connect.
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Unable to listen.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Socket is listening.\n");

    // As the socket is in listen mode there is a connection request pending.

    struct sockaddr_in client_addr;
    int n_client_addr = sizeof(client_addr);
    SOCKET client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &n_client_addr);
    if (client_socket == INVALID_SOCKET)
    {
        printf("accept() failed.\n");
    }
    printf("Client connected\n");

    // store the client's info

    struct CLIENT_INFO client_info;
    client_info.client_addr = client_addr;
    client_info.client_socket = client_socket;

    // output of the client's addr
    printf("Client connected from %s\n", inet_ntoa(client_addr.sin_addr));

    // store the oncoming data
    char buffer[BUFFER_SIZE];

    while (true)
    {
        // receiving message from the client
        int len = recv(client_socket, (char *)&buffer, BUFFER_SIZE, 0);

        if (len > 0) {
            buffer[len] = '\0';
            printf("From %s: %s\n", inet_ntoa(client_info.client_addr.sin_addr), buffer);
        } else {
            printf("No message\n");
        }

        printf("Server: ");

        fgets(buffer, BUFFER_SIZE, stdin);
        send(client_socket, (const char *)&buffer, strlen(buffer), 0);
    }
    printf("Exiting...");
    closesocket(server_socket);
    closesocket(client_socket);
    WSACleanup();

    return 0;
}

bool init_winsock()
{
    WSADATA wsa;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsa) == 0)
    {
        return true;
    }

    return false;
}