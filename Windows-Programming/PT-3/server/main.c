#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <processthreadsapi.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 4503
#define IP "0.0.0.0"
#define MAX_NAME_LEN 100
#define BUF_SIZE 1024
#define MAX_CLIENTS 255

typedef struct {
    char name[MAX_NAME_LEN];
    SOCKET clientSocket;
    struct sockaddr_in clientAddr;
} ClientInfo;

ClientInfo clientList[MAX_CLIENTS];
CRITICAL_SECTION lock;
int clientCount = 0;

void sendBroadcast(const char* msg) {
    EnterCriticalSection(&lock);
    for (int i = 0; i < clientCount; i++) {
        if (send(clientList[i].clientSocket, msg, strlen(msg), 0) == SOCKET_ERROR) {
            printf("Send failed to client %s: %d\n", clientList[i].name, WSAGetLastError());
        }
    }
    LeaveCriticalSection(&lock);
}

void removeClient(int index) {
    EnterCriticalSection(&lock);
    // pthread_mutex_lock(&clientMutex);
    for (int i = index; i < clientCount - 1; i++) {
        clientList[i] = clientList[i + 1];
    }
    clientCount--;
    LeaveCriticalSection(&lock);
    // pthread_mutex_unlock(&clientMutex);
}

void* handle_client(void* arg) {
    ClientInfo* clientInfo = (ClientInfo*)arg;
    SOCKET clientSocket = clientInfo->clientSocket;
    char name[MAX_NAME_LEN] = {0};
    char buffer[BUF_SIZE] = {0};
    int iResult;
    int clientIndex;

    iResult = recv(clientSocket, name, MAX_NAME_LEN - 1, 0);
    if (iResult <= 0) {
        printf("Failed to receive name or client disconnected: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        free(clientInfo);
        return NULL;
    }
    name[iResult] = '\0';

    if (strlen(name) == 0 || strspn(name, " \t\n\r") == strlen(name)) {
        printf("Invalid or empty name received.\n");
        send(clientSocket, "Invalid name", 12, 0);
        closesocket(clientSocket);
        free(clientInfo);
        return NULL;
    }

    EnterCriticalSection(&lock);
    if (clientCount >= MAX_CLIENTS) {
        printf("Client limit reached, rejecting connection.\n");
        send(clientSocket, "Server full", 11, 0);
        closesocket(clientSocket);
        free(clientInfo);
        LeaveCriticalSection(&lock);
        // pthread_mutex_unlock(&clientMutex);
        return NULL;
    }
    strcpy(clientInfo->name, name);
    clientList[clientCount] = *clientInfo;
    clientIndex = clientCount;
    clientCount++;
    LeaveCriticalSection(&lock);
    // pthread_mutex_unlock(&clientMutex);

    printf("New client connected: %s (%s:%d)\n",
           clientInfo->name,
           inet_ntoa(clientInfo->clientAddr.sin_addr),
           ntohs(clientInfo->clientAddr.sin_port));

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        iResult = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (iResult == SOCKET_ERROR) {
            printf("%s encountered error: %d\n", name, WSAGetLastError());
            break;
        }
        if (iResult == 0) {
            printf("%s disconnected.\n", name);
            break;
        }

        buffer[iResult] = '\0';
        char message[BUF_SIZE];
        int written = snprintf(message, BUF_SIZE, "%s: %s\n", name, buffer);
        if (written >= BUF_SIZE) {
            printf("Warning: Message from %s truncated.\n", name);
        }
        printf("%s", message);
        sendBroadcast(message);

        if (strcmp(buffer, "quit") == 0) {
            send(clientSocket, "exit", 4, 0);
            break;
        }
    }

    closesocket(clientSocket);
    removeClient(clientIndex);
    free(clientInfo);
    return NULL;
}

int main() {
    InitializeCriticalSection(&lock);
    WSADATA wsaData;
    SOCKET listenSocket;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("Socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &serverAddr.sin_addr);
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on %s:%d\n", IP, PORT);

    while (1) {
        ClientInfo* clientInfo = (ClientInfo*)malloc(sizeof(ClientInfo));
        if (!clientInfo) {
            printf("Memory allocation failed.\n");
            continue;
        }
        int clientLen = sizeof(clientInfo->clientAddr);
        clientInfo->clientSocket = accept(listenSocket, (struct sockaddr*)&clientInfo->clientAddr, &clientLen);

        if (clientInfo->clientSocket == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            free(clientInfo);
            continue;
        }

        HANDLE tid;

        if (CreateThread(NULL, 0, handle_client, clientInfo, 0, &tid) == NULL) {
            printf("Thread creation failed.\n");
            closesocket(clientInfo->clientSocket);
            free(clientInfo);
            continue;
        }
        CloseHandle(tid);
    }

    closesocket(listenSocket);
    WSACleanup();
    DeleteCriticalSection(&lock);
    return 0;
}