#include <windows.h>
#include "Shellhook.h"

#if _WIN64
#define HOOKLIBNAME L"ShellHook.x64.dll"
#else
#define HOOKLIBNAME L"ShellHook.dll"
#endif

InstallProc install_;
UninstallProc uninstall_;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	HMODULE dll = LoadLibrary(HOOKLIBNAME);
	if (dll == NULL)
		return 2;
	
	install_ = (InstallProc)GetProcAddress(dll, "install");
	uninstall_ = (UninstallProc)GetProcAddress(dll, "uninstall");

	install_();
	
	// we can use a message loop without a window
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		if (msg.message == WM_CLOSE) {
			break;
		}
	}
	uninstall_();
	FreeLibrary(dll);
	return 0;
}