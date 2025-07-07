#include <stdbool.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 1024

bool running = true;


DWORD WINAPI  receive_messages(void* arg) {
    SOCKET connectionSocket = *(SOCKET*)arg;
    char recvbuf[BUF_SIZE];
    int iResult;

    while (running) {
        iResult = recv(connectionSocket, recvbuf, sizeof(recvbuf) - 1, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("\n %s \n", recvbuf);
        } else if (iResult == 0) {
            printf("Server disconnected.\n");
            running = false;
            break;
        } else {
            printf("recv failed: %d\n", WSAGetLastError());
            running = false;
            break;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    int iResult;

    if (argc < 4 || argc > 5) {
        printf("Usage: %s <IP> <port> <your name>\n", argv[0]);
        return 0;
    }
    char* ip = argv[1];
    char* port = argv[2];
    char *username = argv[3];

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo hints, *result = NULL, *ptr = NULL;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(ip, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerror(iResult));
        WSACleanup();
        return 1;
    }

    SOCKET connectionSocket = INVALID_SOCKET;

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        connectionSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectionSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        iResult = connect(connectionSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connectionSocket);
            connectionSocket = INVALID_SOCKET;
            continue;
        }
        send(connectionSocket, username, (int)strlen(username), 0);
        break;
    }

    freeaddrinfo(result);

    if (connectionSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    HANDLE recv_thread;
    DWORD threadId;
    recv_thread = CreateThread(NULL, 0, receive_messages, &connectionSocket, 0, &threadId);
    if (recv_thread == NULL) {
        printf("Failed to create receive thread.\n");
        closesocket(connectionSocket);
        WSACleanup();
        return 1;
    }

    char sendbuf[BUF_SIZE];
    do {
        if (fgets(sendbuf, sizeof(sendbuf), stdin) == NULL) {
            printf("Error reading input or EOF.\n");
            break;
        }

        sendbuf[strcspn(sendbuf, "\n")] = 0;

        if (strcmp(sendbuf, "quit") == 0) {
            iResult = send(connectionSocket, "user has disconnected", 4, 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
            }

            iResult = shutdown(connectionSocket, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                printf("shutdown failed: %d\n", WSAGetLastError());
            }
            break;
        }
        iResult = send(connectionSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(connectionSocket);
            WSACleanup();
            return 1;
        }
    } while (true);

    closesocket(connectionSocket);
    WSACleanup();
    return 0;
}