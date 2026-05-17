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

    TabProjects,
    TabSteam,
    TabLogs,

    ProjectsTitle,
    SaveSettings,
    NewProject,
    SelectBuildFolder,
    ProjectList,
    SelectedProject,
    ProjectName,
    ProjectRoot,
    BuildDirectory,
    Version,
    UploadTargets,
    LastUpdated,
    Changelog,
    DeleteProject,
    NoProjects,
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

    LogProjectsBuildFolder,
    LogProjectsSave,
    LogProjectsSelected,
    LogProjectsCreated,
    LogProjectsDeleted,
    LogProjectsNoProjectSelected,
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
