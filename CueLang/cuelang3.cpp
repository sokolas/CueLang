// cuelang3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "CUESDK.h"

using LedColorsVector = std::vector<CorsairLedColor>;

DWORD childThread32 = 0;
DWORD childThread64 = 0;

HWND hwnd;
HINSTANCE hInstance;

LedColorsVector leds1, leds2;

const wchar_t CLASS_NAME[] = L"CueLangApp";
std::wstring childexe = L"HookSupportApp.exe";
std::wstring childexe64 = L"HookSupportApp.x64.exe";

// input language handles, they seem to be consistent between system reboots
LPARAM ru = 68748313;
LPARAM en = 67699721;

// RGB values for languages
const int en_r = 132;
const int en_g = 199;
const int en_b = 255;

const int  ru_r = 255;
const int  ru_g = 55;
const int  ru_b = 10;


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    BOOL exit = FALSE;
    BOOL result = FALSE;

    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        printf("Ctrl-C event\n\n");
        
        exit = TRUE;
        result = TRUE;
        break;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        printf("Ctrl-Close event\n\n");
        exit = TRUE;
        result = TRUE;
        break;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        printf("Ctrl-Break event\n\n");
        exit = TRUE;
        result = FALSE;
        break;

    case CTRL_LOGOFF_EVENT:
        printf("Ctrl-Logoff event\n\n");
        exit = true;
        result = FALSE;
        break;

    case CTRL_SHUTDOWN_EVENT:
        printf("Ctrl-Shutdown event\n\n");
        exit = true;
        result = FALSE;
        break;

    default:
        result = FALSE;
    }

    if (exit != FALSE) {
        if (childThread32 != 0) {
            PostThreadMessage(childThread32, WM_CLOSE, 0, 0);
        }
        if (childThread64 != 0) {
            PostThreadMessage(childThread64, WM_CLOSE, 0, 0);
        }
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }

    return result;
}

void changeLang(WPARAM wParam, LPARAM lParam)
{
    //std::cout << wParam << ":" << lParam << "\n";
    if (lParam == ru)
        CorsairSetLedsColorsBufferByDeviceIndex(0, static_cast<int>(leds1.size()), leds2.data());
    else
        CorsairSetLedsColorsBufferByDeviceIndex(0, static_cast<int>(leds1.size()), leds1.data());
    CorsairSetLedsColorsFlushBuffer();
}

LRESULT CALLBACK WndProc(
    _In_ HWND hWnd,
    _In_ UINT message,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    switch (message)
    {
    case WM_USER + 1:
        changeLang(wParam, lParam);
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

const char* toString(CorsairError error)
{
    switch (error) {
    case CE_Success:
        return "CE_Success";
    case CE_ServerNotFound:
        return "CE_ServerNotFound";
    case CE_NoControl:
        return "CE_NoControl";
    case CE_ProtocolHandshakeMissing:
        return "CE_ProtocolHandshakeMissing";
    case CE_IncompatibleProtocol:
        return "CE_IncompatibleProtocol";
    case CE_InvalidArguments:
        return "CE_InvalidArguments";
    default:
        return "unknown error";
    }
}

void getAllLeds()
{
    for (auto deviceIndex = 0; deviceIndex < CorsairGetDeviceCount(); deviceIndex++) {
        if (const auto ledPositions = CorsairGetLedPositionsByDeviceIndex(deviceIndex)) {
            for (auto i = 0; i < ledPositions->numberOfLed; i++) {
                const auto ledId = ledPositions->pLedPosition[i].ledId;
                leds1.push_back(CorsairLedColor{ ledId, en_r, en_g, en_b });
                leds2.push_back(CorsairLedColor{ ledId, ru_r, ru_g, ru_b });
            }
        }
    }
}

int main()
{
    CorsairPerformProtocolHandshake();
    if (const auto error = CorsairGetLastError()) {
        std::cout << "Handshake failed: " << toString(error) << std::endl;
        return 2;
    }

    getAllLeds();

    hInstance = GetModuleHandle(NULL);

    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        printf("\nERROR: Could not set control handler");
        return 1;
    }

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"CueLangApp",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (hwnd == NULL) {
        return 0;
    }
    ShowWindow(hwnd, SW_HIDE);

    // child processes
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    STARTUPINFO si64;
    PROCESS_INFORMATION pi64;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    ZeroMemory(&si64, sizeof(si64));
    si64.cb = sizeof(si64);
    ZeroMemory(&pi64, sizeof(pi64));

 
    if (!CreateProcess(NULL,   // No module name (use command line)
        &childexe[0],        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess 32 failed (%d).\n", GetLastError());
        return 1;
    }

    WaitForSingleObject(pi.hProcess, 100);
    DWORD exitCode;
    
    if (FALSE == GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        std::cerr << "GetExitCodeProcess() failure: " <<
            GetLastError() << "\n";
    }
    else if (STILL_ACTIVE == exitCode)
    {
        std::cout << "Still running\n";
    }
    else
    {
        std::cout << "exit code=" << exitCode << "\n";
    }

    childThread32 = GetThreadId(pi.hThread);

    if (!CreateProcess(NULL,   // No module name (use command line)
        &childexe64[0],        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si64,            // Pointer to STARTUPINFO structure
        &pi64)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess x64 failed (%d).\n", GetLastError());
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return 1;
    }

    WaitForSingleObject(pi64.hProcess, 100);
    if (FALSE == GetExitCodeProcess(pi64.hProcess, &exitCode))
    {
        std::cerr << "GetExitCodeProcess() failure: " <<
            GetLastError() << "\n";
    }
    else if (STILL_ACTIVE == exitCode)
    {
        std::cout << "Still running\n";
    }
    else
    {
        std::cout << "exit code=" << exitCode << "\n";
    }

    childThread64 = GetThreadId(pi64.hThread);

    // msg loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (childThread32 != 0) {
        // Wait until child process exits.
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles. 
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    if (childThread64 != 0) {
        // Wait until child process exits.
        WaitForSingleObject(pi64.hProcess, INFINITE);

        // Close process and thread handles. 
        CloseHandle(pi64.hProcess);
        CloseHandle(pi64.hThread);
    }

    return 0;
}