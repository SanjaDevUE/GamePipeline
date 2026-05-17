#include "localization/Language.h"

namespace localization {
QString translate(Language language, Text text)
{
    const bool german = language == Language::German;

    switch (text) {
    case Text::AppStarted:
        return german ? "GamePipeline Steam Uploader gestartet." : "GamePipeline Steam Uploader started.";
    case Text::Ready:
        return german ? "Bereit" : "Ready";

    case Text::SaveSettings:
        return german ? "Einstellungen speichern" : "Save Settings";
    case Text::Project:
        return german ? "Projekt" : "Project";
    case Text::Browse:
        return german ? "Durchsuchen" : "Browse";

    case Text::SteamTitle:
        return german ? "Steam Uploader" : "Steam Uploader";
    case Text::ContentBuilderPath:
        return german ? "Steamworks SDK ContentBuilder-Pfad" : "Steamworks SDK ContentBuilder path";
    case Text::SteamworksSdkPath:
        return german ? "Steamworks SDK-Ordner" : "Steamworks SDK folder";
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
        return german ? "Depot hinzufuegen" : "Add Depot";
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
        return german ? "Log-Ordner oeffnen" : "View Logs Folder";
    case Text::UpdateSteamSdk:
        return german ? "ContentBuilder aktualisieren" : "Update ContentBuilder";
    case Text::SteamProgressReady:
        return german ? "Bereit" : "Ready";
    case Text::SteamProgressValidating:
        return german ? "Konfiguration pruefen" : "Validating config";
    case Text::SteamProgressValidated:
        return german ? "Konfiguration gueltig" : "Config valid";
    case Text::SteamProgressSaving:
        return german ? "Projekt speichern" : "Saving project";
    case Text::SteamProgressSaved:
        return german ? "Projekt gespeichert" : "Project saved";
    case Text::SteamProgressGenerating:
        return german ? "VDF-Dateien erzeugen" : "Generating VDF files";
    case Text::SteamProgressGenerated:
        return german ? "VDF-Dateien erzeugt" : "VDF files generated";
    case Text::SteamProgressUploading:
        return german ? "Steam-Upload laeuft" : "Steam upload running";
    case Text::SteamProgressComplete:
        return german ? "Abgeschlossen" : "Complete";
    case Text::SteamProgressFailed:
        return german ? "Fehlgeschlagen" : "Failed";
    case Text::ValidateConfig:
        return german ? "Konfiguration pruefen" : "Validate Config";
    case Text::GenerateVdf:
        return german ? "VDF erzeugen" : "Generate VDF";
    case Text::UploadToSteam:
        return german ? "Zu Steam hochladen" : "Upload to Steam";

    case Text::LogsTitle:
        return german ? "Logs" : "Logs";
    case Text::LogOutput:
        return german ? "Log-Ausgabe" : "Log Output";
    case Text::Clear:
        return german ? "Leeren" : "Clear";

    case Text::LogSteamProjectSaved:
        return german ? "Steam: Projektkonfiguration fuer %1 gespeichert."
                      : "Steam: saved project configuration for %1.";
    case Text::LogSteamValidate:
        return german ? "Steam: Konfiguration geprueft."
                      : "Steam: validated config.";
    case Text::LogSteamVdfGenerated:
        return german ? "Steam: VDF-Dateien in %1 erzeugt." : "Steam: generated VDF files in %1.";
    case Text::LogSteamUploadStarted:
        return german ? "Steam: Upload gestartet." : "Steam: upload started.";
    case Text::LogSteamUploadFinished:
        return german ? "Steam: Upload beendet." : "Steam: upload finished.";
    case Text::LogSteamUploadFailed:
        return german ? "Steam: Upload konnte nicht gestartet werden: %1"
                      : "Steam: upload could not be started: %1";
    case Text::LogSteamDepotAdded:
        return german ? "Steam: Depot hinzugefuegt." : "Steam: depot added.";
    case Text::LogSteamDepotRemoved:
        return german ? "Steam: Depot entfernt." : "Steam: depot removed.";
    case Text::LogSteamSdkUpdated:
        return german ? "Steam: ContentBuilder aus %1 aktualisiert."
                      : "Steam: updated ContentBuilder from %1.";
    case Text::LogSteamSdkUpdateFailed:
        return german ? "Steam: ContentBuilder konnte nicht aktualisiert werden: %1"
                      : "Steam: ContentBuilder could not be updated: %1";
    }

    return {};
}
}
