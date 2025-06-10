#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

#define MAX_PATH_LEN 260

int main() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Cannot create Snapshot (%lu)\n", GetLastError());
        return 1;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        printf("Error: Cannot take the first process (%lu)\n", GetLastError());
        CloseHandle(hSnapshot);
        return 1;
    }

    printf("Process list:\n");
    printf("------------------------------------------------------------------------------------------\n");
    printf("%-20s | %30s | %10s | %30s\n", "Process ID", "Process Name", "Parent ID", "Parent Name");

    do {
        DWORD pid = pe32.th32ProcessID;
        DWORD ppid = pe32.th32ParentProcessID;
        CHAR* processName = pe32.szExeFile;

        char parentName[MAX_PATH_LEN] = "<unknown>";

        HANDLE hParentSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hParentSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 peParent;
            peParent.dwSize = sizeof(PROCESSENTRY32);

            if (Process32First(hParentSnap, &peParent)) {
                do {
                    if (peParent.th32ProcessID == ppid) {
                        strcpy_s(parentName, MAX_PATH_LEN, peParent.szExeFile);
                        break;
                    }
                } while (Process32Next(hParentSnap, &peParent));
            }
            CloseHandle(hParentSnap);
        }

        printf("%-20lu | %30s | %10lu | %30s\n", pid, processName, ppid, parentName);

    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return 0;
}
