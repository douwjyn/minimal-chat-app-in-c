#include <stdio.h>
#include <WinSock2.h>
#include <stdbool.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_ADDR "127.0.0.1" // localhost
#define BUFFER_SIZE 512

int main(void) {
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Error initializing.");
        return 1;
    }

    // create socket
    SOCKET c_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (c_socket == INVALID_SOCKET) {
        printf("Error creating socket.");
        return 1;
    }

    // bind socket to an address
    struct sockaddr_in c_addr;

    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(7878);
    c_addr.sin_addr.s_addr = inet_addr(DEFAULT_ADDR);

    // connect to a socket
    if (connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) == SOCKET_ERROR) {
        printf("Unable to connect to the server: %d", WSAGetLastError());
        closesocket(c_socket);
        WSACleanup();
        return 1;
    }
    printf("Connected to %s\n", DEFAULT_ADDR);

    // this buffer will store the 'message' being sent and received
    char buffer[BUFFER_SIZE];

    // start the infinite loop
    while (true) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        // send message to the connected socket/address
        int message = send(c_socket, (const char *)&buffer,  BUFFER_SIZE, 0);

        // if (message > 0) {
            // printf("message delivered.\n");
        // }

        int received = recv(c_socket, (char *)&buffer, BUFFER_SIZE, 0);
        
        // the recv function returns the length in bytes of the received data if im right
        if (received > 0) {
            // set a null terminator to the received data to make it a valid string type
            buffer[received] = '\0';
            printf("Server: %s\n", buffer);
        } else {
            printf("Server shutdown...");
            break;
        }
    }

    closesocket(c_socket);
    WSACleanup();


    return 0;
}