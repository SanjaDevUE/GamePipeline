# GamePipeline

GamePipeline is a Windows-first Qt6 desktop Steam uploader for game developers.


<img width="1211" height="801" alt="grafik" src="https://github.com/user-attachments/assets/909981c9-5e08-48cf-aa34-96e2ce55bc70" />

## Stack

- C++20
- Qt6 Widgets
- CMake
- Ninja / CLion

## Features

- Project management with saved build and deployment settings
- SteamPipe VDF generation and SteamCMD upload workflow
- Portable Steamworks SDK ContentBuilder tools
- ContentBuilder updates from a locally downloaded Steamworks SDK

## Project Layout

```text
src/
  domain/      Core application data models
  ui/          Qt Widgets windows and views
resources/
  styles/      Embedded QSS styles
```

## Planned Features

- SteamCMD integration
- Automated build uploads
- Branch and depot management
- Version tracking
- Upload history and logs
- Build compression and validation
- Simple and modern UI

## Build

```powershell
cmake --preset windows-mingw-debug
cmake --build --preset windows-mingw-debug
```
