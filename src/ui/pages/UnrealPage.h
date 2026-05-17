#pragma once

#include "localization/Language.h"

#include <QWidget>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QProcess;
class QProgressBar;
class ProjectRepository;
class QPushButton;

class UnrealPage final : public QWidget
{
    Q_OBJECT

public:
    explicit UnrealPage(ProjectRepository *repository, QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

signals:
    void logRequested(const QString &message);

private:
    void refreshProjects(int preferredRow = -1);
    void loadProject(int comboIndex);
    void prepareDraftProject();
    void clearUnrealForm();
    void setUnrealFormEnabled(bool enabled);
    void saveCurrentProject();
    void browseEnginePath();
    void browseProjectPath();
    void browseArchiveDirectory();
    bool persistCurrentProject(bool writeLog);
    bool validateConfiguration();
    void packageProject();
    void openArchiveDirectory();
    void appendPackageLog(const QString &message);
    void appendProcessOutput();
    void setProgress(int value, const QString &text, const QString &state = QStringLiteral("active"));
    void setProgressBusy(const QString &text);
    void setProgressError(const QString &text);
    [[nodiscard]] int currentProjectRow() const;
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    ProjectRepository *m_repository = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_projectLabel = nullptr;
    QLabel *m_pathsSectionLabel = nullptr;
    QLabel *m_buildSectionLabel = nullptr;
    QLabel *m_enginePathLabel = nullptr;
    QLabel *m_projectPathLabel = nullptr;
    QLabel *m_archiveDirectoryLabel = nullptr;
    QLabel *m_platformLabel = nullptr;
    QLabel *m_configurationLabel = nullptr;
    QLabel *m_additionalArgumentsLabel = nullptr;
    QComboBox *m_projectComboBox = nullptr;
    QLineEdit *m_enginePathEdit = nullptr;
    QLineEdit *m_projectPathEdit = nullptr;
    QLineEdit *m_archiveDirectoryEdit = nullptr;
    QLineEdit *m_additionalArgumentsEdit = nullptr;
    QComboBox *m_platformComboBox = nullptr;
    QComboBox *m_configurationComboBox = nullptr;
    QProgressBar *m_progressBar = nullptr;
    QCheckBox *m_buildCheckBox = nullptr;
    QCheckBox *m_cookCheckBox = nullptr;
    QCheckBox *m_stageCheckBox = nullptr;
    QCheckBox *m_pakCheckBox = nullptr;
    QCheckBox *m_archiveCheckBox = nullptr;
    QCheckBox *m_cleanCheckBox = nullptr;
    QCheckBox *m_distributionCheckBox = nullptr;
    QPushButton *m_saveProjectButton = nullptr;
    QPushButton *m_browseEngineButton = nullptr;
    QPushButton *m_browseProjectButton = nullptr;
    QPushButton *m_browseArchiveButton = nullptr;
    QPushButton *m_validateButton = nullptr;
    QPushButton *m_packageButton = nullptr;
    QPushButton *m_openArchiveButton = nullptr;
    QProcess *m_packageProcess = nullptr;
};
