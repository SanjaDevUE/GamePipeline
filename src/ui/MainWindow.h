#pragma once

#include "localization/Language.h"

#include <QMainWindow>

class LogsPage;
class ProjectRepository;
class QSplitter;
class SteamPage;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void writeLog(const QString &message);

private:
    void setupUi();
    void connectPageLogs();
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    ProjectRepository *m_projectRepository = nullptr;
    QSplitter *m_splitter = nullptr;
    SteamPage *m_steamPage = nullptr;
    LogsPage *m_logsPage = nullptr;
};
