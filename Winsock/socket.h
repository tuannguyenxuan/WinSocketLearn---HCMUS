#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#include <iostream>
using namespace std;
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream> 
#include<vector>
#include<thread>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1000
#define DEFAULT_PORT "80"


int InitWSA(WSADATA& wsaData);  // Initialize Winsock

int CreateSocketServer(addrinfo& hints, addrinfo*& result, SOCKET& ListenSocket); // Create socket for server

int Bind(addrinfo*& result, SOCKET& ListenSocket); // Setup the TCP listening socket

int Listen(SOCKET ListenSocket);  // Listen the connection from the client

void ExcuteFunc(SOCKET ClientSocket, char recvbuf[], int recvbuflen, int port); // Receive and send data on the Server

void Shutdown(SOCKET &ClientSocket);  //Disconnecting the Server