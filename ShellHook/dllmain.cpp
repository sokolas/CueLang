#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "Shellhook.h"

static HHOOK hook;  // installed hook handle, need it to uninstall it
static HMODULE module;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    module = hModule;
    return TRUE;
}

// this function is called every time a user changes imput language
extern "C" LRESULT CALLBACK hookproc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0) // do not process message
        return CallNextHookEx(hook, nCode,
            wParam, lParam);
    switch (nCode)
    {
    case HSHELL_LANGUAGE:
    {
        HWND wnd = FindWindow(L"CueLangApp", L"CueLangApp");    // we're hard-coding the strings here for simplicity
        if (wnd != NULL)
            PostMessage(wnd, WM_USER + 1, wParam, lParam);
    }
    default:
        break;
    }


    return CallNextHookEx(hook, nCode, wParam, lParam);
}

extern "C" SHELLHOOK_API void install()
{
    hook = SetWindowsHookEx(WH_SHELL, hookproc, module, 0);
}

extern "C" SHELLHOOK_API void uninstall()
{
    UnhookWindowsHookEx(hook);
}

