#include <windows.h>
#include <stdio.h>
#include <dbghelp.h> 
#pragma comment(lib, "dbghelp.lib")

typedef int (WINAPI* MessageBoxAType)(HWND, LPCSTR, LPCSTR, UINT);
MessageBoxAType originalMessageBoxA = NULL;

int WINAPI hookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
	MessageBoxW(NULL, L"Hoook called by nguyendh", L"Hooked", MB_OK);
    return originalMessageBoxA(hWnd, lpText, lpCaption, uType);
}

void HookIAT() {
    HMODULE hModule = GetModuleHandle(NULL);
    ULONG size;
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
        hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size);

    for (; importDesc->Name; importDesc++) {
        LPCSTR moduleName = (LPCSTR)((PBYTE)hModule + importDesc->Name);
        if (_stricmp(moduleName, "user32.dll") != 0)
            continue;

        PIMAGE_THUNK_DATA thunkOrig = (PIMAGE_THUNK_DATA)((PBYTE)hModule + importDesc->OriginalFirstThunk);
        PIMAGE_THUNK_DATA thunkFirst = (PIMAGE_THUNK_DATA)((PBYTE)hModule + importDesc->FirstThunk);

        for (; thunkOrig->u1.AddressOfData; thunkOrig++, thunkFirst++) {
            PIMAGE_IMPORT_BY_NAME import = (PIMAGE_IMPORT_BY_NAME)((PBYTE)hModule + thunkOrig->u1.AddressOfData);
            if (strcmp((char*)import->Name, "MessageBoxA") == 0) {
                DWORD oldProtect;
                VirtualProtect(&thunkFirst->u1.Function, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &oldProtect);
                originalMessageBoxA = (MessageBoxAType)(thunkFirst->u1.Function);
                thunkFirst->u1.Function = (ULONG_PTR)hookedMessageBoxA;
                VirtualProtect(&thunkFirst->u1.Function, sizeof(LPVOID), oldProtect, &oldProtect);
                break;
            }
        }
    }
}

int main() {
    HookIAT();
    MessageBoxA(NULL, "Test", "Hooked", MB_OK);
    return 0;
}
