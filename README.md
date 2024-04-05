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

## как собрать
Вам нужна Visual Studio community edition, проверено с версией 2022. Установщик спросит, какие компоненты нужны, выберите "разработка C++".
https://learn.microsoft.com/ru-ru/visualstudio/install/install-visual-studio?view=vs-2022
Также на вкладке с компонентами должен быть `windows sdk 142` или новее. После установки надо будет распаковать архив с исходниками куда-нибудь, открыть CueLang.sln студией, если предложит обновить компоненты — обновите. Затем надо будет сверху в окошке выбрать Release и x64 и запустить Build -> Build solution (или нажать ctrl-shift-b. В папке с проектом появится папка Release. Для работы надо только exe и dll файлы, можно скопировать их куда-нибудь отдельно. Для запуска — CueLang.exe

https://github.com/sokolas/CueLang/blob/master/CueLang/cuelang3.cpp#L29 тут можно поменять цвета, каждое значение от 1 до 255
