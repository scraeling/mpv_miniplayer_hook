#include "pch.h"
#include <Windows.h>
#include "winuser.h"
#include <algorithm>

struct WindowPosition {
	int x;
	int y;
	int width;
	int height;
};

RECT DesktopRect;
RECT WindowRect;
WindowPosition LastWindowPos;
WindowPosition MiniplayerPos;
LONG_PTR OldWndProc = NULL;

LRESULT CALLBACK __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_ACTIVATE: { // Window focused
		switch (wParam) {
		case WA_ACTIVE:
		case WA_CLICKACTIVE: {
			SetWindowPos(hWnd, HWND_TOP, LastWindowPos.x, LastWindowPos.y, LastWindowPos.width, LastWindowPos.height, NULL);
			break;
		}
		case WA_INACTIVE: { //Window loses focus
			GetWindowRect(hWnd, &WindowRect);
			LastWindowPos = { WindowRect.left, WindowRect.top, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top };
			SetWindowPos(hWnd, HWND_TOPMOST, MiniplayerPos.x, MiniplayerPos.y, MiniplayerPos.width, MiniplayerPos.height, NULL);
			break;
		}
		default: break;
		}
	}
	default: break;
	}

	return CallWindowProcW((WNDPROC)OldWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		// Get a handle to the window
		HWND WindowHandle = FindWindow(L"mpv", NULL); // Get a handle to our desired window.

		// Calculate miniplayer size based on screensize 
		GetWindowRect(GetDesktopWindow(), &DesktopRect);
		GetWindowRect(WindowHandle, &WindowRect);
		int height = WindowRect.bottom - WindowRect.top;
		int width = WindowRect.right - WindowRect.left;
		int padding = round((float)height / (float)48);
		double ratio = (float)width / (float)height;
		MiniplayerPos.height = round((float)(DesktopRect.bottom - DesktopRect.top) / (float)4);
		MiniplayerPos.width = round(ratio * float(MiniplayerPos.height));
		MiniplayerPos.x = DesktopRect.right - MiniplayerPos.width - padding;
		MiniplayerPos.y = DesktopRect.bottom - MiniplayerPos.height - padding;

		LastWindowPos = { WindowRect.left, WindowRect.top, width, height }; // Save current window size and position
		OldWndProc = SetWindowLongPtr(WindowHandle, GWLP_WNDPROC, (LONG_PTR)WndProc); // Replace WndProc with our custom one.
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}