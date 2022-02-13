#pragma once

#ifdef SHELLHOOK_EXPORTS
#define SHELLHOOK_API __declspec(dllexport)
#else
#define SHELLHOOK_API __declspec(dllimport)
#endif

extern "C" SHELLHOOK_API void install();
extern "C" SHELLHOOK_API void uninstall();

// we need these typedefs to use GetProcAddress dynamically
typedef void (__cdecl *InstallProc)();
typedef void(__cdecl *UninstallProc)();
