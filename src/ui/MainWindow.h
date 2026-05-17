#pragma once

#include "localization/Language.h"

#include <QMainWindow>

class DashboardPage;
class ItchPage;
class LogsPage;
class ProjectRepository;
class ProjectsPage;
class SettingsPage;
class SteamPage;
class QTabWidget;
class UnrealPage;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void writeLog(const QString &message);
    void setLanguage(localization::Language language);

private:
    void setupUi();
    void connectPageLogs();
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    ProjectRepository *m_projectRepository = nullptr;
    QTabWidget *m_tabs = nullptr;
    DashboardPage *m_dashboardPage = nullptr;
    ProjectsPage *m_projectsPage = nullptr;
    UnrealPage *m_unrealPage = nullptr;
    SteamPage *m_steamPage = nullptr;
    ItchPage *m_itchPage = nullptr;
    LogsPage *m_logsPage = nullptr;
    SettingsPage *m_settingsPage = nullptr;
};
