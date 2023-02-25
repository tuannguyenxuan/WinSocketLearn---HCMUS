#include"socket.h"


int InitWSA(WSADATA& wsaData)
{
   // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 0;
    }
}
int CreateSocketServer(addrinfo& hints, addrinfo*& result, SOCKET& ListenSocket)
{
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port

    int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 0;
    }
    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
}
int Bind(addrinfo*& result, SOCKET& ListenSocket)
{
    // Binding a Socket
    // Setup the TCP listening socket
    int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

}
int Listen(SOCKET ListenSocket)
{
    //Listening on a Socket
    int iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
}
void ExcuteFunc(SOCKET ClientSocket, char recvbuf[], int recvbuflen, int port)
{
    int iResult = 0;
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            // Parse out the client's request string e.g. GET /index.html HTTP/1.1
            istringstream iss(recvbuf);
            vector<string> parsed((istream_iterator<string>(iss)), istream_iterator<string>());

            // Some defaults for output to the client (404 file not found 'page')
            string sendbuf = "";
            string Filename = "";
            int errorCode = 200;

            // If the GET request is valid, try and get the name
            if (parsed.size() >= 3 && (parsed[0] == "GET" || parsed[0] == "POST"))
            {
                Filename = parsed[1];
                // If the file is just a slash, use index.html. This should really
                // be if it _ends_ in a slash. I'll leave that for you :)
                if (Filename == "/")
                {
                    errorCode = 301;
                }
                Filename.erase(Filename.begin(), Filename.begin() + 1);
            }
            if (parsed[0] == "POST")
            {

                string logininfo = parsed[parsed.size() - 1];
                size_t separator = logininfo.find_first_of("&");
                string user = logininfo.substr(5, separator - 5);
                string pass = logininfo.substr(separator + 6, logininfo.size() - separator - 6);
                if (user == "admin" && pass == "admin") {}
                else
                {
                    errorCode = 302;
                    Filename = "404.html";
                }
            }
            cout << "\nClient Port " << port << " request " << Filename << endl;
            ifstream f;
            f.open(Filename, ifstream::binary);
            if (f.good())
            {
                string str((std::istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
                sendbuf = str;
            }
            f.close();

            std::ostringstream oss;
            oss << "HTTP/1.1 " << errorCode;
            switch (errorCode) {
            case 301:
                oss << " Moved Permanently\r\n";
                oss << "Location: /index.html\r\n";
                break;
            case 302:
                oss << " Found\r\n";
                oss << "Location: /404.html\r\n";
                break;
            default:
                oss << " OK\r\n";
            }

            oss << "Cache-Control: no-cache, private\r\n";
            //Content Type
            string content_type = "";
            for (int i = 0; i < parsed.size(); i++) if (parsed[i] == "Accept:") {
                content_type = parsed[i + 1];
                break;
            }
            size_t found = content_type.find_first_of(",");
            content_type = content_type.substr(0, found);
            oss << "Content-Type: " << content_type << "\r\n";
            //
            oss << "Content-Length: " << sendbuf.size() << "\r\n";
            oss << "\r\n";
            oss << sendbuf;
            string output = oss.str();

            // cout << output << endl;

            int iSendResult = send(ClientSocket, output.c_str(), output.length(), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }

        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }
    } while (iResult > 0);

    // shutdown the connection since we're done

    return;
}
void Shutdown(SOCKET& ClientSocket)
{
    int iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return;
    }
}

