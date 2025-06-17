#include <stdio.h>
#include <windows.h>
#include <winnt.h>

typedef int (WINAPI* MessageBoxAType)(HWND, LPCSTR, LPCSTR, UINT);

MessageBoxAType originalMessageBoxA = MessageBoxA;

int WINAPI messageBoxHook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    printf("hehehe");
    return originalMessageBoxA(hWnd, "Hooked by nguyendh", "Hooked", MB_OK);
}

int main() {
    HMODULE hModule = GetModuleHandle(NULL);
    if (!hModule) {
        printf("GetModuleHandle failed\n");
        return 1;
    }
    MessageBoxA(NULL, "Test before hooking", "Test", MB_OK);
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);

    DWORD importRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (!importRVA) {
        printf("No import directory found\n");
        return 1;
    }

    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule + importRVA);

    while (importDesc->Name) {
        const char* dllName = (const char*)((BYTE*)hModule + importDesc->Name);
        if (_stricmp(dllName, "user32.dll") == 0) {
            PIMAGE_THUNK_DATA thunkOrig = (PIMAGE_THUNK_DATA)((BYTE*)hModule + importDesc->OriginalFirstThunk);
            PIMAGE_THUNK_DATA thunkFirst = (PIMAGE_THUNK_DATA)((BYTE*)hModule + importDesc->FirstThunk);

            while (thunkOrig->u1.AddressOfData) {
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)hModule + thunkOrig->u1.AddressOfData);
                if (strcmp(funcName->Name, "MessageBoxA") == 0) {
                    DWORD oldProtect;
                    VirtualProtect(&thunkFirst->u1.Function, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &oldProtect);
                    thunkFirst->u1.Function = (ULONGLONG)messageBoxHook;
                    VirtualProtect(&thunkFirst->u1.Function, sizeof(LPVOID), oldProtect, &oldProtect);
                    printf("Hooked MessageBoxA successfully.\n");
                    break;
                }
                ++thunkOrig;
                ++thunkFirst;
            }
        }
        ++importDesc;
    }

    MessageBoxA(NULL, "Test after hooking", "Test", MB_OK);

    return 0;
}