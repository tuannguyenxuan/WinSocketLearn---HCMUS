#include"socket.h"

int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock

    InitWSA(wsaData);

    // Create socket for server
    CreateSocketServer(hints, result, ListenSocket);

    // Binding a Socket
    // Setup the TCP listening socket
    Bind(result, ListenSocket);
  //  accept(SOCKET s,sockaddr* addr,int * addrlen)


   //Listening on a Socket
    Listen(ListenSocket);
    
    while (1)
    {
        socklen_t addr_size;
        struct sockaddr_in client;
        addr_size = sizeof(struct sockaddr_in);
        // Accept a client socket
        ClientSocket = accept(ListenSocket, (struct sockaddr*)&client, &addr_size);
        cout <<"\nClient connection established with PORT :" << ntohs(client.sin_port) << endl;

        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            break;
        }
        // Receive and send data on the Server
        thread(ExcuteFunc, ClientSocket, recvbuf, recvbuflen, ntohs(client.sin_port)).detach();

    }

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    // No longer need server socket

    return 0;
}