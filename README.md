# GamePipeline

Windows-first Qt6 desktop application for managing indie game build and upload workflows.

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

## Build

```powershell
cmake --preset windows-mingw-debug
cmake --build --preset windows-mingw-debug
```
