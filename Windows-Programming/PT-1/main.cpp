#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <timezoneapi.h>
#include <winbase.h>
#include <shlwapi.h>


void showHelp(char* filename) {
    printf("Usage: %s [options] [Directory path]\n", filename);
    printf("Options:\n");
    printf("\t--help/-h\t\tShow this help message\n");
    printf("\t--show/-s\t\tShow file in the sub folder \n");
}

boolean checkPermission(const char* path, ACCESS_MASK desiredAccess) {
    HANDLE hFile = CreateFileA(path, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        return true;
    }
    return false;
}

void list(char* path, bool isShow) {
    WIN32_FIND_DATAA findFileData;

    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*", path);

    HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Could not open directory %s\n", path);
    }
    printf("Directory: %s\n", path);
    printf("%8s %12s %13s %50s\n", "Mode", "LastWriteTime", "Length", "Name");
    do {
        SYSTEMTIME sys;
        FileTimeToSystemTime(&findFileData.ftLastWriteTime, &sys);
        char permission[] = "----";
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            permission[0] = 'd';
            printf("%8s %02u/%02u/%4u %02u:%02u %10s %50s\n", permission, sys.wDay, sys.wMonth, sys.wYear,sys.wHour,sys.wMinute,"",findFileData.cFileName);
            if (isShow) {
                char directory[MAX_PATH];
                snprintf(directory, MAX_PATH, "%s/%s", path, findFileData.cFileName);
                list(directory, true);
            }
        }
        else {
            char fullPath[MAX_PATH];
            snprintf(fullPath, MAX_PATH, "%s\\%s", path, findFileData.cFileName);
            permission[1] = checkPermission(fullPath, GENERIC_READ) == true ? 'r' : '-';
            permission[2] = checkPermission(fullPath, GENERIC_WRITE) == true ? 'w' : '-';
            permission[3] = checkPermission(fullPath, GENERIC_EXECUTE) == true ? 'x' : '-';
            printf("%8s %02u/%02u/%4u %02u:%02u %10lu %50s\n",
                   permission,sys.wDay, sys.wMonth, sys.wYear,
                   sys.wHour, sys.wMinute,
                   findFileData.nFileSizeLow,
                   findFileData.cFileName);
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);
}


int main(int argc, char* argv[]) {
    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        showHelp(argv[0]);
    }
    else if (argc > 1 && (strcmp(argv[1], "--show") == 0 || strcmp(argv[1], "-s") == 0)) {
        char *path = argv[2];
        list(path, true);
    }
    else {
        char *path = NULL;
        if ( argc == 2) {
            path = argv[1];
        }else {
            path = ".";
        }
        list(path, false);
    }
    return 0;
}