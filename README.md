# GamePipeline

GamePipeline is a Windows-first Qt6 desktop tool focused on Steam build uploads.

The app keeps the scope intentionally narrow: manage multiple games, generate SteamPipe VDF files, run SteamCMD uploads, and keep the bundled Steamworks `ContentBuilder` tools up to date from a locally downloaded Steamworks SDK.

## Features

- Manage multiple game projects from the `Projects` view
- Store per-project Steam App ID, depots, build paths, branch, credentials, and build description
- Generate SteamPipe app/depot VDF files
- Upload builds through SteamCMD
- Keep logs in a dedicated `Logs` view
- Bundle portable Steam ContentBuilder tools with the app
- Update `Steam/ContentBuilder` from a locally extracted Steamworks SDK

## Steamworks SDK Updates

Steamworks SDK downloads are available through the Steamworks partner portal. When Valve releases a new SDK:

1. Download and extract the latest Steamworks SDK.
2. Open GamePipeline.
3. Go to the `Steam` view.
4. Click `Update ContentBuilder`.
5. Select either the extracted SDK root or its `sdk/tools/ContentBuilder` folder.

The updater refreshes the bundled SteamPipe tool files and keeps these local working folders intact:

- `content`
- `scripts`
- `output`

## Scope

GamePipeline is now Steam-only. Unreal packaging, itch.io uploads, and unrelated deployment targets are intentionally out of scope.

## Stack

- C++20
- Qt6 Widgets
- CMake
- Ninja / CLion

## Project Layout

```text
src/
  domain/      Project storage and Steam configuration models
  ui/          Qt Widgets windows and pages
resources/
  styles/      Embedded QSS styles
Steam/
  ContentBuilder/ Portable SteamPipe tools copied into builds
```

## Build

```powershell
cmake --preset windows-mingw-debug
cmake --build --preset windows-mingw-debug
```
