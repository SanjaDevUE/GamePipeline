# GamePipeline

GamePipeline is a Windows-first Qt6 desktop application for game developers that simplifies managing and uploading game builds for platforms like Steam and itch.io.


<img width="1211" height="801" alt="grafik" src="https://github.com/user-attachments/assets/909981c9-5e08-48cf-aa34-96e2ce55bc70" />

## Stack

- C++20
- Qt6 Widgets
- CMake
- Ninja / CLion

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
- itch.io Butler support
- Automated build uploads
- Branch and depot management
- Version tracking
- Changelog editor
- Unreal Engine packaging support
- Upload history and logs
- Discord webhook notifications
- Multi-platform deployment
- Build compression and validation
- Simple and modern UI

## Build

```powershell
cmake --preset windows-mingw-debug
cmake --build --preset windows-mingw-debug
```
