# CueLang

A sample program to control Corsair LED color based on selected input language.
> NB: only working configurations are: win32/x64 Release for ShellHook, win32/x64 Release for HookSupportApp, win32 Release for CueLang!

## ShellHook
DLL that installs and uninstalls global WH_SHELL hook. There are two build configurations, x86 and x64, because a 32-bit DLL can only be injected in 32-bit processes, and 64-bit DLL into 64-bit processes.

## HookSupportApp
A windowless app that loads a DLL with the hook callback and registers and deregisters the hook.

## CueLang
The main app that loads and controls both x86 and x64 versions of HookSupportApp. All the Corsair-specific stuff happens here. LIB, DLL and include files related to Corsair are from this project: https://github.com/CorsairOfficial/cue-sdk

See also:
* https://corsairofficial.github.io/cue-sdk/
