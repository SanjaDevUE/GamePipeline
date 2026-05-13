#include "localization/Language.h"

namespace localization {
QString translate(Language language, Text text)
{
    const bool german = language == Language::German;

    switch (text) {
    case Text::AppStarted:
        return german ? "GamePipeline gestartet." : "GamePipeline started.";
    case Text::LanguageChanged:
        return german ? "Sprache auf %1 umgestellt." : "Language changed to %1.";
    case Text::Ready:
        return german ? "Bereit" : "Ready";

    case Text::TabDashboard:
        return german ? "Dashboard" : "Dashboard";
    case Text::TabProjects:
        return german ? "Projekte" : "Projects";
    case Text::TabSteam:
        return "Steam";
    case Text::TabItch:
        return "itch.io";
    case Text::TabLogs:
        return german ? "Logs" : "Logs";
    case Text::TabSettings:
        return german ? "Einstellungen" : "Settings";

    case Text::DashboardTitle:
        return german ? "Dashboard" : "Dashboard";
    case Text::ManagedProjects:
        return german ? "Verwaltete Projekte" : "Managed projects";
    case Text::ReadyBuilds:
        return german ? "Fertige Builds" : "Ready builds";
    case Text::SteamUploads:
        return german ? "Steam-Uploads" : "Steam uploads";
    case Text::ItchUploads:
        return german ? "itch.io-Uploads" : "itch.io uploads";
    case Text::Placeholder:
        return german ? "Platzhalter" : "Placeholder";
    case Text::QuickActions:
        return german ? "Schnellaktionen" : "Quick Actions";
    case Text::NewProject:
        return german ? "Neues Projekt" : "New Project";
    case Text::SelectBuildFolder:
        return german ? "Build-Ordner auswählen" : "Select Build Folder";
    case Text::PrepareSteamUpload:
        return german ? "Steam-Upload vorbereiten" : "Prepare Steam Upload";
    case Text::PrepareItchUpload:
        return german ? "itch.io-Upload vorbereiten" : "Prepare itch.io Upload";

    case Text::ProjectsTitle:
        return german ? "Projekte" : "Projects";
    case Text::SaveSettings:
        return german ? "Einstellungen speichern" : "Save Settings";
    case Text::ProjectList:
        return german ? "Projektliste" : "Project List";
    case Text::SelectedProject:
        return german ? "Ausgewähltes Projekt" : "Selected Project";
    case Text::ProjectPrefix:
        return german ? "Projekt: %1" : "Project: %1";
    case Text::BuildPrefix:
        return german ? "Build: %1" : "Build: %1";
    case Text::VersionPrefix:
        return german ? "Version: %1" : "Version: %1";
    case Text::TargetsPrefix:
        return german ? "Ziele: %1" : "Targets: %1";
    case Text::UpdatedPrefix:
        return german ? "Aktualisiert: %1" : "Updated: %1";
    case Text::Changelog:
        return german ? "Änderungsprotokoll" : "Changelog";

    case Text::SteamTitle:
        return "Steam";
    case Text::SteamCmdPlaceholder:
        return german ? "SteamCMD-Platzhalter" : "SteamCMD Placeholder";
    case Text::SteamCmdPath:
        return german ? "SteamCMD-Pfad" : "SteamCMD path";
    case Text::AppId:
        return german ? "App-ID" : "App ID";
    case Text::DepotId:
        return german ? "Depot-ID" : "Depot ID";
    case Text::BuildDescription:
        return german ? "Build-Beschreibung" : "Build description";
    case Text::ValidateConfig:
        return german ? "Konfiguration prüfen" : "Validate Config";
    case Text::GenerateVdf:
        return german ? "VDF erzeugen" : "Generate VDF";
    case Text::UploadToSteam:
        return german ? "Zu Steam hochladen" : "Upload to Steam";

    case Text::ItchTitle:
        return "itch.io";
    case Text::ButlerPlaceholder:
        return german ? "Butler-Platzhalter" : "Butler Placeholder";
    case Text::ButlerPath:
        return german ? "Butler-Pfad" : "Butler path";
    case Text::UserGame:
        return german ? "Benutzer/Spiel" : "User/game";
    case Text::Channel:
        return german ? "Kanal" : "Channel";
    case Text::Version:
        return german ? "Version" : "Version";
    case Text::CheckStatus:
        return german ? "Status prüfen" : "Check Status";
    case Text::PushWithButler:
        return german ? "Mit Butler hochladen" : "Push with Butler";

    case Text::LogsTitle:
        return german ? "Logs" : "Logs";
    case Text::LogOutput:
        return german ? "Log-Ausgabe" : "Log Output";
    case Text::Clear:
        return german ? "Leeren" : "Clear";

    case Text::SettingsTitle:
        return german ? "Einstellungen" : "Settings";
    case Text::ApplicationSettings:
        return german ? "Anwendungseinstellungen" : "Application Settings";
    case Text::WorkspaceFolder:
        return german ? "Workspace-Ordner" : "Workspace folder";
    case Text::Language:
        return german ? "Sprache" : "Language";
    case Text::Theme:
        return german ? "Design" : "Theme";
    case Text::UseDarkTheme:
        return german ? "Dunkles Theme verwenden" : "Use dark theme";
    case Text::English:
        return german ? "Englisch" : "English";
    case Text::German:
        return german ? "Deutsch" : "Deutsch";

    case Text::LogDashboardNewProject:
        return german ? "Dashboard: Platzhalter für neues Projekt geklickt."
                      : "Dashboard: new project placeholder clicked.";
    case Text::LogDashboardBuildFolder:
        return german ? "Dashboard: Platzhalter für Build-Ordner-Auswahl geklickt."
                      : "Dashboard: build folder selection placeholder clicked.";
    case Text::LogDashboardSteamUpload:
        return german ? "Dashboard: Platzhalter für Steam-Upload-Vorbereitung geklickt."
                      : "Dashboard: Steam upload preparation placeholder clicked.";
    case Text::LogDashboardItchUpload:
        return german ? "Dashboard: Platzhalter für itch.io-Upload-Vorbereitung geklickt."
                      : "Dashboard: itch.io upload preparation placeholder clicked.";
    case Text::LogProjectsNewProject:
        return german ? "Projekte: Platzhalter für neues Projekt geklickt."
                      : "Projects: new project placeholder clicked.";
    case Text::LogProjectsBuildFolder:
        return german ? "Projekte: Platzhalter für Build-Ordner-Auswahl geklickt."
                      : "Projects: build folder selection placeholder clicked.";
    case Text::LogProjectsSave:
        return german ? "Projekte: Platzhalter für Speichern der Einstellungen geklickt."
                      : "Projects: settings save placeholder clicked.";
    case Text::LogProjectsSelected:
        return german ? "Projekte: %1 ausgewählt." : "Projects: selected %1.";
    case Text::LogSteamValidate:
        return german ? "Steam: Platzhalter für Konfigurationsprüfung geklickt."
                      : "Steam: validate config placeholder clicked.";
    case Text::LogSteamGenerateVdf:
        return german ? "Steam: Platzhalter für VDF-Erzeugung geklickt."
                      : "Steam: VDF generation placeholder clicked.";
    case Text::LogSteamUpload:
        return german ? "Steam: Upload-Platzhalter geklickt."
                      : "Steam: upload placeholder clicked.";
    case Text::LogItchValidate:
        return german ? "itch.io: Platzhalter für Konfigurationsprüfung geklickt."
                      : "itch.io: validate config placeholder clicked.";
    case Text::LogItchStatus:
        return german ? "itch.io: Platzhalter für Statusprüfung geklickt."
                      : "itch.io: status check placeholder clicked.";
    case Text::LogItchUpload:
        return german ? "itch.io: Butler-Upload-Platzhalter geklickt."
                      : "itch.io: Butler push placeholder clicked.";
    case Text::LogSettingsSave:
        return german ? "Einstellungen: Speicherplatzhalter geklickt."
                      : "Settings: save placeholder clicked.";
    }

    return {};
}

QString languageName(Language language, Language displayLanguage)
{
    return translate(displayLanguage, language == Language::German ? Text::German : Text::English);
}
}
