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
    LanguageChanged,
    Ready,

    TabDashboard,
    TabProjects,
    TabSteam,
    TabItch,
    TabLogs,
    TabSettings,

    DashboardTitle,
    ManagedProjects,
    ReadyBuilds,
    SteamUploads,
    ItchUploads,
    Placeholder,
    QuickActions,
    NewProject,
    SelectBuildFolder,
    PrepareSteamUpload,
    PrepareItchUpload,

    ProjectsTitle,
    SaveSettings,
    ProjectList,
    SelectedProject,
    ProjectPrefix,
    BuildPrefix,
    VersionPrefix,
    TargetsPrefix,
    UpdatedPrefix,
    Changelog,

    SteamTitle,
    SteamCmdPlaceholder,
    SteamCmdPath,
    AppId,
    DepotId,
    BuildDescription,
    ValidateConfig,
    GenerateVdf,
    UploadToSteam,

    ItchTitle,
    ButlerPlaceholder,
    ButlerPath,
    UserGame,
    Channel,
    Version,
    CheckStatus,
    PushWithButler,

    LogsTitle,
    LogOutput,
    Clear,

    SettingsTitle,
    ApplicationSettings,
    WorkspaceFolder,
    Language,
    Theme,
    UseDarkTheme,
    English,
    German,

    LogDashboardNewProject,
    LogDashboardBuildFolder,
    LogDashboardSteamUpload,
    LogDashboardItchUpload,
    LogProjectsNewProject,
    LogProjectsBuildFolder,
    LogProjectsSave,
    LogProjectsSelected,
    LogSteamValidate,
    LogSteamGenerateVdf,
    LogSteamUpload,
    LogItchValidate,
    LogItchStatus,
    LogItchUpload,
    LogSettingsSave,
};

[[nodiscard]] QString translate(Language language, Text text);
[[nodiscard]] QString languageName(Language language, Language displayLanguage);
}

Q_DECLARE_METATYPE(localization::Language)
