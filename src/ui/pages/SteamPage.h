#pragma once

#include "domain/GameProject.h"
#include "localization/Language.h"

#include <QWidget>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QProcess;
class QTableWidget;
class ProjectRepository;
class QPushButton;

class SteamPage final : public QWidget
{
    Q_OBJECT

public:
    explicit SteamPage(ProjectRepository *repository, QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

signals:
    void logRequested(const QString &message);

private:
    void refreshProjects(int preferredRow = -1);
    void loadProject(int comboIndex);
    void prepareDraftProject();
    void clearSteamForm();
    void setSteamFormEnabled(bool enabled);
    void saveCurrentProject();
    void addDepot();
    void removeDepot();
    void browseSelectedDepotPath();
    void browseContentBuilderPath();
    bool persistCurrentProject(bool writeLog);
    bool validateConfiguration();
    bool generateVdfs();
    void uploadToSteam();
    void openLogsFolder();
    void appendUploadLog(const QString &message);
    void appendProcessOutput();
    void setProgress(int value, const QString &text, const QString &state = QStringLiteral("active"));
    void setProgressBusy(const QString &text);
    void setProgressError(const QString &text);
    void updatePasswordEchoMode();
    [[nodiscard]] int currentProjectRow() const;
    [[nodiscard]] QVector<SteamDepotConfig> collectDepots(bool portable) const;
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    ProjectRepository *m_repository = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_projectLabel = nullptr;
    QLabel *m_depotConfigurationLabel = nullptr;
    QLabel *m_steamConfigurationLabel = nullptr;
    QLabel *m_contentBuilderPathLabel = nullptr;
    QLabel *m_appIdLabel = nullptr;
    QLabel *m_buildDescriptionLabel = nullptr;
    QLabel *m_branchLabel = nullptr;
    QLabel *m_loginLabel = nullptr;
    QLabel *m_passwordLabel = nullptr;
    QComboBox *m_projectComboBox = nullptr;
    QLineEdit *m_appIdEdit = nullptr;
    QLineEdit *m_buildDescriptionEdit = nullptr;
    QLineEdit *m_branchEdit = nullptr;
    QLineEdit *m_contentBuilderPathEdit = nullptr;
    QLineEdit *m_loginEdit = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    QTableWidget *m_depotTable = nullptr;
    QProgressBar *m_progressBar = nullptr;
    QCheckBox *m_previewBuildCheckBox = nullptr;
    QCheckBox *m_showPasswordCheckBox = nullptr;
    QCheckBox *m_savePasswordCheckBox = nullptr;
    QPushButton *m_saveProjectButton = nullptr;
    QPushButton *m_validateButton = nullptr;
    QPushButton *m_addDepotButton = nullptr;
    QPushButton *m_removeDepotButton = nullptr;
    QPushButton *m_browseDepotButton = nullptr;
    QPushButton *m_browseContentBuilderButton = nullptr;
    QPushButton *m_generateButton = nullptr;
    QPushButton *m_uploadButton = nullptr;
    QPushButton *m_viewLogsFolderButton = nullptr;
    QProcess *m_uploadProcess = nullptr;
};
