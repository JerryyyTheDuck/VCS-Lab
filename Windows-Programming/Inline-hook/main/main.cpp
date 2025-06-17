#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>

int findProcessID(int pid) {
    if (pid <= 0) {
        printf("Invalid PID: %d\n", pid);
        return -1;
    }

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot failed: %d\n", GetLastError());
        return -1;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        printf("Process32First failed: %d\n", GetLastError());
        CloseHandle(hProcessSnap);
        return -1;
    }

    do {
        if (pe32.th32ProcessID == (DWORD)pid) {
            CloseHandle(hProcessSnap);
            return pid;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    printf("Process with PID %d not found\n", pid);
    return -1;
}


bool injectDLL(int pid, const char* dllPath) {
    if (GetFileAttributesA(dllPath) == INVALID_FILE_ATTRIBUTES) {
        printf("DLL file not found: %s\n", dllPath);
        return false;
    }

    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid);
    if (hProcess == NULL) {
        printf("OpenProcess failed: %d (Are you running as admin?)\n", GetLastError());
        return false;
    }

    size_t pathLen = strlen(dllPath) + 1;
    void* pRemoteBuf = VirtualAllocEx(hProcess, NULL, pathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (pRemoteBuf == NULL) {
        printf("VirtualAllocEx failed: %d\n", GetLastError());
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, pRemoteBuf, (void*)dllPath, pathLen, NULL)) {
        printf("WriteProcessMemory failed: %d\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");
    if (loadLibraryAddr == NULL) {
        printf("GetProcAddress failed: %d\n", GetLastError()); 
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, loadLibraryAddr, pRemoteBuf, 0, NULL);
    if (hThread == NULL) {
        printf("CreateRemoteThread failed: %d\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }



    WaitForSingleObject(hThread, INFINITE);

    DWORD exitCode;
    if (!GetExitCodeThread(hThread, &exitCode)) {
        printf("GetExitCodeThread failed: %d\n", GetLastError());
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    printf("LoadLibrary returned: 0x%X\n", exitCode);

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return exitCode != NULL;
}


////////////
// Idea 1 //
///////////

//int main() {
//    int pid = 0;
//	char dllPath[MAX_PATH] = "C:\\Users\\Jerryyy\\source\\repos\\test\\x64\\Debug\\CreateFIleWNew.dll";
//    printf("Enter process ID to inject into: ");
//    if (scanf_s("%d", &pid) != 1) {
//        printf("Invalid PID input\n");
//        return 1;
//    }
//
//    int foundPid = findProcessID(pid);
//    if (foundPid == -1) {
//        return 1;
//    }
//
//    printf("Attempting to inject %s into PID %d...\n", dllPath, foundPid);
//    if (injectDLL(foundPid, dllPath)) {
//        printf("DLL injected successfully!\n");
//    }
//    else {
//        printf("DLL injection failed\n");
//    }
//
//    return 0;
//}

////////////
// Idea 2 //
///////////

bool checkIfInjected(DWORD pid, DWORD* injectedPIDs, int injectedCount) {
    for (int i = 0; i < injectedCount; i++) {
        if (injectedPIDs[i] == pid) {
            return true;
        }
    }
    return false;
}

int main(int argc, char argv[]) {
    DWORD injectedPIDs[1024] = { 0 };
    int injectedCount = 0;
	char dllPath[MAX_PATH] = "C:\\Users\\Jerryyy\\source\\repos\\test\\x64\\Debug\\CreateFIleWNew.dll";
    while(true) {
		DWORD currentNotepadPID[1024] = { 0 };
		int currentNotepadCount = 0;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            printf("CreateToolhelp32Snapshot failed: %d\n", GetLastError());
        }

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (!Process32First(hSnapshot, &pe32)) {
            printf("Process32First failed: %d\n", GetLastError());
            CloseHandle(hSnapshot);
        }
        do {
            if (wcscmp(pe32.szExeFile, L"notepad.exe") == 0) {
				currentNotepadPID[currentNotepadCount++] = pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));
        CloseHandle(hSnapshot);
        
        for (int i = 0; i < currentNotepadCount; i++) {
            DWORD pid = currentNotepadPID[i];
            if (!checkIfInjected(pid, injectedPIDs, injectedCount)) {
                injectDLL(pid, dllPath);
            }
        }

        memcpy(injectedPIDs, currentNotepadPID, currentNotepadCount * sizeof(DWORD));
        injectedCount = currentNotepadCount;

        if (injectedCount < 1024) {
            memset(&injectedPIDs[injectedCount], 0, (1024 - injectedCount) * sizeof(DWORD));
        }
		printf("Currently injected PIDs: %d\n", injectedCount);
        Sleep(1000);
	}
        
    return 0;
}