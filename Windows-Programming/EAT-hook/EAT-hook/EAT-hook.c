#include<stdio.h>
#include<Windows.h>

typedef int (WINAPI* messageBoxAHook)(HWND, LPCSTR, LPCSTR, UINT);

int WINAPI hookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
	printf("Hooked MessageBoxA called!\n");
	return originalMessageBoxA(hWnd, lpText, lpCaption, uType);
}


messageBoxAHook originalMessageBoxA = NULL;



int main(int arg, char* argv[]) {
	HMODULE ImageBase = LoadLibraryA("user32.dll");
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)dosHeader + dosHeader->e_lfanew);
	
	DWORD exportRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;


	PIMAGE_EXPORT_DIRECTORY exportDescriptor = (PIMAGE_EXPORT_DIRECTORY)((DWORD_PTR)ImageBase + exportRVA);

	DWORD NumberOfNames = exportDescriptor->NumberOfNames;
	DWORD* AddressOfNames = (DWORD*)((DWORD_PTR)ImageBase + exportDescriptor->AddressOfNames);
	WORD* AddressOfNameOrdinals = (WORD*)((DWORD_PTR)ImageBase + exportDescriptor->AddressOfNameOrdinals);
	DWORD* AddressOfFunctions = (DWORD*)((DWORD_PTR)ImageBase + exportDescriptor->AddressOfFunctions);

	for (DWORD i = 0; i < NumberOfNames; i++) {
		char* functionName = (char*)((DWORD_PTR)ImageBase + AddressOfNames[i]);
		if (strcmp(functionName, "MessageBoxA") == 0) {
			WORD ordinal = AddressOfNameOrdinals[i];
			DWORD* addressOfMessageBox = &AddressOfFunctions[ordinal]; 
			originalMessageBoxA = (messageBoxAHook)((DWORD_PTR)ImageBase + *addressOfMessageBox);
			DWORD oldProtect;
			VirtualProtect(addressOfMessageBox, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
			*addressOfMessageBox = (DWORD)hookedMessageBoxA;
			VirtualProtect(addressOfMessageBox, sizeof(DWORD), oldProtect, &oldProtect);
			break;
		}
	}

	MessageBoxA(NULL, "OK", "OK", MB_OK);	
	return 0; 
}