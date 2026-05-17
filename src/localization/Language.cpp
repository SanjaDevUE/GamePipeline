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
    case Text::TabUnreal:
        return "Unreal";
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
    case Text::ProjectName:
        return german ? "Projektname" : "Project name";
    case Text::ProjectRoot:
        return german ? "Projektordner" : "Project root";
    case Text::BuildDirectory:
        return german ? "Build-Ordner" : "Build directory";
    case Text::UploadTargets:
        return german ? "Upload-Ziele" : "Upload targets";
    case Text::LastUpdated:
        return german ? "Zuletzt gespeichert" : "Last saved";
    case Text::DeleteProject:
        return german ? "Projekt löschen" : "Delete Project";
    case Text::NoProjects:
        return german ? "Noch keine Projekte gespeichert." : "No projects saved yet.";
    case Text::Project:
        return german ? "Projekt" : "Project";
    case Text::Browse:
        return german ? "Durchsuchen" : "Browse";

    case Text::SteamTitle:
        return "Steam";
    case Text::SteamProject:
        return german ? "Steam-Projekt" : "Steam Project";
    case Text::SteamCmdPlaceholder:
        return german ? "SteamCMD-Platzhalter" : "SteamCMD Placeholder";
    case Text::SteamCmdPath:
        return german ? "SteamCMD-Pfad" : "SteamCMD path";
    case Text::ContentBuilderPath:
        return german ? "Steamworks SDK ContentBuilder-Pfad" : "Steamworks SDK ContentBuilder path";
    case Text::AppId:
        return german ? "App-ID" : "App ID";
    case Text::DepotId:
        return german ? "Depot-ID" : "Depot ID";
    case Text::BuildPath:
        return german ? "Build-Pfad" : "Build path";
    case Text::BuildDescription:
        return german ? "Build-Beschreibung" : "Build description";
    case Text::DepotConfiguration:
        return german ? "Depot-Konfiguration" : "Depot Configuration";
    case Text::SteamConfiguration:
        return german ? "Steam-Konfiguration" : "Steam Configuration";
    case Text::Branch:
        return german ? "Live-Branch" : "Live branch";
    case Text::PreviewBuild:
        return german ? "Preview-Build" : "Preview build";
    case Text::AddDepot:
        return german ? "Depot hinzufügen" : "Add Depot";
    case Text::RemoveDepot:
        return german ? "Depot entfernen" : "Remove Depot";
    case Text::SteamLogin:
        return german ? "Steam-Login" : "Steam login";
    case Text::SteamPassword:
        return german ? "Steam-Passwort" : "Steam password";
    case Text::ShowPassword:
        return german ? "Passwort anzeigen" : "Show Password";
    case Text::SavePassword:
        return german ? "Passwort speichern" : "Save Password";
    case Text::ViewLogsFolder:
        return german ? "Log-Ordner öffnen" : "View Logs Folder";
    case Text::SteamProgressReady:
        return german ? "Bereit" : "Ready";
    case Text::SteamProgressValidating:
        return german ? "Konfiguration prüfen" : "Validating config";
    case Text::SteamProgressValidated:
        return german ? "Konfiguration gültig" : "Config valid";
    case Text::SteamProgressSaving:
        return german ? "Projekt speichern" : "Saving project";
    case Text::SteamProgressSaved:
        return german ? "Projekt gespeichert" : "Project saved";
    case Text::SteamProgressGenerating:
        return german ? "VDF-Dateien erzeugen" : "Generating VDF files";
    case Text::SteamProgressGenerated:
        return german ? "VDF-Dateien erzeugt" : "VDF files generated";
    case Text::SteamProgressUploading:
        return german ? "Steam-Upload läuft" : "Steam upload running";
    case Text::SteamProgressComplete:
        return german ? "Abgeschlossen" : "Complete";
    case Text::SteamProgressFailed:
        return german ? "Fehlgeschlagen" : "Failed";
    case Text::ValidateConfig:
        return german ? "Konfiguration prüfen" : "Validate Config";
    case Text::GenerateVdf:
        return german ? "VDF erzeugen" : "Generate VDF";
    case Text::UploadToSteam:
        return german ? "Zu Steam hochladen" : "Upload to Steam";

    case Text::UnrealTitle:
        return "Unreal Engine";
    case Text::UnrealConfiguration:
        return german ? "Unreal-Konfiguration" : "Unreal Configuration";
    case Text::UnrealEnginePath:
        return german ? "Unreal-Engine-Pfad" : "Unreal Engine path";
    case Text::UnrealProjectFile:
        return german ? "Unreal-Projektdatei" : "Unreal project file";
    case Text::UnrealArchiveDirectory:
        return german ? "Paket-Ausgabeordner" : "Package output folder";
    case Text::UnrealPlatform:
        return german ? "Zielplattform" : "Target platform";
    case Text::UnrealBuildConfiguration:
        return german ? "Build-Konfiguration" : "Build configuration";
    case Text::UnrealBuildOptions:
        return german ? "Packaging-Optionen" : "Packaging Options";
    case Text::UnrealAdditionalArguments:
        return german ? "Zusaetzliche Argumente" : "Additional arguments";
    case Text::UnrealBuild:
        return "Build";
    case Text::UnrealCook:
        return "Cook";
    case Text::UnrealStage:
        return "Stage";
    case Text::UnrealPak:
        return "Pak";
    case Text::UnrealArchive:
        return "Archive";
    case Text::UnrealClean:
        return "Clean";
    case Text::UnrealDistribution:
        return "Distribution";
    case Text::UnrealPackageProject:
        return german ? "Projekt paketieren" : "Package Project";
    case Text::UnrealOpenArchiveFolder:
        return german ? "Ausgabeordner oeffnen" : "Open Output Folder";
    case Text::UnrealProgressReady:
        return german ? "Bereit" : "Ready";
    case Text::UnrealProgressValidating:
        return german ? "Konfiguration pruefen" : "Validating config";
    case Text::UnrealProgressValidated:
        return german ? "Konfiguration gueltig" : "Config valid";
    case Text::UnrealProgressSaving:
        return german ? "Projekt speichern" : "Saving project";
    case Text::UnrealProgressSaved:
        return german ? "Projekt gespeichert" : "Project saved";
    case Text::UnrealProgressPackaging:
        return german ? "Unreal-Packaging laeuft" : "Unreal packaging running";
    case Text::UnrealProgressComplete:
        return german ? "Abgeschlossen" : "Complete";
    case Text::UnrealProgressFailed:
        return german ? "Fehlgeschlagen" : "Failed";

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
        return german ? "Projekte: Build-Ordner ausgewählt."
                      : "Projects: build folder selected.";
    case Text::LogProjectsSave:
        return german ? "Projekte: %1 gespeichert."
                      : "Projects: saved %1.";
    case Text::LogProjectsSelected:
        return german ? "Projekte: %1 ausgewählt." : "Projects: selected %1.";
    case Text::LogProjectsCreated:
        return german ? "Projekte: Neues Projekt angelegt." : "Projects: created new project.";
    case Text::LogProjectsDeleted:
        return german ? "Projekte: %1 gelöscht." : "Projects: deleted %1.";
    case Text::LogProjectsNoProjectSelected:
        return german ? "Projekte: Kein Projekt ausgewählt." : "Projects: no project selected.";
    case Text::LogSteamProjectSaved:
        return german ? "Steam: Projektkonfiguration für %1 gespeichert."
                      : "Steam: saved project configuration for %1.";
    case Text::LogSteamNoProjectSelected:
        return german ? "Steam: Kein Projekt ausgewählt." : "Steam: no project selected.";
    case Text::LogSteamValidate:
        return german ? "Steam: Konfiguration geprüft."
                      : "Steam: validated config.";
    case Text::LogSteamGenerateVdf:
        return german ? "Steam: VDF-Erzeugung gestartet."
                      : "Steam: VDF generation started.";
    case Text::LogSteamVdfGenerated:
        return german ? "Steam: VDF-Dateien in %1 erzeugt." : "Steam: generated VDF files in %1.";
    case Text::LogSteamUpload:
        return german ? "Steam: Upload vorbereitet."
                      : "Steam: upload prepared.";
    case Text::LogSteamUploadStarted:
        return german ? "Steam: Upload gestartet." : "Steam: upload started.";
    case Text::LogSteamUploadFinished:
        return german ? "Steam: Upload beendet." : "Steam: upload finished.";
    case Text::LogSteamUploadFailed:
        return german ? "Steam: Upload konnte nicht gestartet werden: %1"
                      : "Steam: upload could not be started: %1";
    case Text::LogSteamDepotAdded:
        return german ? "Steam: Depot hinzugefügt." : "Steam: depot added.";
    case Text::LogSteamDepotRemoved:
        return german ? "Steam: Depot entfernt." : "Steam: depot removed.";
    case Text::LogUnrealProjectSaved:
        return german ? "Unreal: Projektkonfiguration fuer %1 gespeichert."
                      : "Unreal: saved project configuration for %1.";
    case Text::LogUnrealValidate:
        return german ? "Unreal: Konfiguration geprueft."
                      : "Unreal: validated config.";
    case Text::LogUnrealPackageStarted:
        return german ? "Unreal: Packaging gestartet." : "Unreal: packaging started.";
    case Text::LogUnrealPackageFinished:
        return german ? "Unreal: Packaging beendet." : "Unreal: packaging finished.";
    case Text::LogUnrealPackageFailed:
        return german ? "Unreal: Packaging konnte nicht gestartet werden: %1"
                      : "Unreal: packaging could not be started: %1";
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
