// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include <stdint.h>

typedef HANDLE(WINAPI* CreateFileWType)(
    LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES,
    DWORD, DWORD, HANDLE);

BYTE originalBytes[5];
CreateFileWType trampoline = NULL;
FARPROC pfn = NULL;

//https://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
//Ghi đè 12 byte cũng được nhưng mà sẽ bị violation lúc tramppoline --> dùng một address gần ghi đè 5 byte làm relay
//Chỗ relay đấy lại jump 12 byte address tới chỗ hook

void* allocatePageNearAddress(void* targetAddr)
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const uint64_t PAGE_SIZE = sysInfo.dwPageSize;

    uint64_t startAddr = ((uint64_t)targetAddr & ~(PAGE_SIZE - 1));
    uint64_t minAddr = (uint64_t)sysInfo.lpMinimumApplicationAddress;
    uint64_t maxAddr = (uint64_t)sysInfo.lpMaximumApplicationAddress;

    uint64_t pageOffset = 1;
    while (1)
    {
        uint64_t byteOffset = pageOffset * PAGE_SIZE;
        uint64_t highAddr = startAddr + byteOffset;
        uint64_t lowAddr = (startAddr > byteOffset) ? startAddr - byteOffset : 0;

        if (highAddr < maxAddr)
        {
            void* outAddr = VirtualAlloc((void*)highAddr, PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (outAddr)
                return outAddr;
        }

        if (lowAddr > minAddr)
        {
            void* outAddr = VirtualAlloc((void*)lowAddr, PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (outAddr)
                return outAddr;
        }

        if (highAddr > maxAddr && lowAddr < minAddr)
            break;

        pageOffset++;
    }

    return NULL;
}

void writeAbsoluteJump(void* memory, void* addrToJumpTo) {
    BYTE jumpTo[] = {
		0x48, 0xB8,                         //mov rax
		0,0,0,0,0,0,0,0,                    // 8 byte address to jump to
		0xFF, 0xE0                          // jmp rax
    };
    memcpy(&jumpTo[2], &addrToJumpTo, sizeof(addrToJumpTo));
    memcpy(memory, jumpTo, sizeof(jumpTo));
}

HANDLE WINAPI CreateFileWHook(
    LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {

    if (wcsstr(lpFileName, L"1.txt") != NULL) {
        MessageBoxA(NULL, "Inline hooked by nguyendh", "Hook", MB_OK);
    }


    DWORD oldProtect;
    VirtualProtect(pfn, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(pfn, originalBytes, 5);
    VirtualProtect(pfn, 5, oldProtect, &oldProtect);

    HANDLE result = CreateFileW(
        lpFileName, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile
    );

    VirtualProtect(pfn, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    uint8_t jmpInstruction[5] = { 0xE9, 0, 0, 0, 0 };
    int32_t relAddr = (int32_t)((uint8_t*)trampoline - ((uint8_t*)pfn + 5));
    memcpy(jmpInstruction + 1, &relAddr, 4);
    memcpy(pfn, jmpInstruction, 5);
    VirtualProtect(pfn, 5, oldProtect, &oldProtect);

    return result;
}

bool installHook(){
    pfn = GetProcAddress(GetModuleHandleA("kernel32.dll"), "CreateFileW");
    if (pfn == NULL) {
        printf("Failed to get address of CreateFileW\n");
        return 1;
    }

    memcpy(originalBytes, pfn, 5); //copy 5 byte đầu vào original byte

    void* relayFuncMemory = allocatePageNearAddress(pfn); // tạo address gần pfn
    if (!relayFuncMemory) {
        printf("Failed to allocate relay function memory\n");
        return 1;
    }
    writeAbsoluteJump(relayFuncMemory, CreateFileWHook); //hook address gần pfn jump tới tới địa chỉ CCreate File Hook
    trampoline = (CreateFileWType)relayFuncMemory; // trỏ trampoline tới relayFuncMemory

    DWORD oldProtect;
    VirtualProtect(pfn, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    uint8_t jmpInstruction[5] = { 0xE9, 0, 0, 0, 0 };
    int32_t relAddr = (int32_t)((uint8_t*)relayFuncMemory - ((uint8_t*)pfn + 5));
    memcpy(jmpInstruction + 1, &relAddr, 4);
    memcpy(pfn, jmpInstruction, 5);
    VirtualProtect(pfn, 5, oldProtect, &oldProtect);
}

DWORD WINAPI HookThread(LPVOID) {
    installHook();
    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, HookThread, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

