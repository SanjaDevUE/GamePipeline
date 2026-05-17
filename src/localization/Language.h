#pragma once

#include <QMetaType>
#include <QString>

namespace localization {
enum class Language
{
    English = 0,
    German = 1,
};

enum class Text
{
    AppStarted,
    Ready,

    SaveSettings,
    Project,
    Browse,

    SteamTitle,
    ContentBuilderPath,
    SteamworksSdkPath,
    AppId,
    DepotId,
    BuildPath,
    BuildDescription,
    DepotConfiguration,
    SteamConfiguration,
    Branch,
    PreviewBuild,
    AddDepot,
    RemoveDepot,
    SteamLogin,
    SteamPassword,
    ShowPassword,
    SavePassword,
    ViewLogsFolder,
    UpdateSteamSdk,
    SteamProgressReady,
    SteamProgressValidating,
    SteamProgressValidated,
    SteamProgressSaving,
    SteamProgressSaved,
    SteamProgressGenerating,
    SteamProgressGenerated,
    SteamProgressUploading,
    SteamProgressComplete,
    SteamProgressFailed,
    ValidateConfig,
    GenerateVdf,
    UploadToSteam,

    LogsTitle,
    LogOutput,
    Clear,

    LogSteamProjectSaved,
    LogSteamValidate,
    LogSteamVdfGenerated,
    LogSteamUploadStarted,
    LogSteamUploadFinished,
    LogSteamUploadFailed,
    LogSteamDepotAdded,
    LogSteamDepotRemoved,
    LogSteamSdkUpdated,
    LogSteamSdkUpdateFailed,
};

[[nodiscard]] QString translate(Language language, Text text);
}

Q_DECLARE_METATYPE(localization::Language)
