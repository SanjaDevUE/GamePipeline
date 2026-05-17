#pragma once

#include "domain/GameProject.h"
#include "localization/Language.h"

#include <QWidget>

class QLabel;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class ProjectRepository;
class QPushButton;

class ProjectsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectsPage(ProjectRepository *repository, QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

signals:
    void logRequested(const QString &message);

private:
    void createProject();
    void saveCurrentProject();
    void deleteCurrentProject();
    void selectBuildFolder();
    void refreshProjectList(int preferredRow = -1);
    void showProject(int row);
    void clearProjectForm();
    void setProjectFormEnabled(bool enabled);
    [[nodiscard]] GameProject projectFromForm(const GameProject &baseProject) const;
    [[nodiscard]] QStringList uploadTargetsFromText() const;
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    ProjectRepository *m_repository = nullptr;
    QListWidget *m_projectList = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_projectListLabel = nullptr;
    QLabel *m_selectedProjectLabel = nullptr;
    QLabel *m_projectNameLabel = nullptr;
    QLabel *m_projectRootLabel = nullptr;
    QLabel *m_buildDirectoryLabel = nullptr;
    QLabel *m_versionLabel = nullptr;
    QLabel *m_targetsLabel = nullptr;
    QLabel *m_lastUpdatedLabel = nullptr;
    QLabel *m_changelogLabel = nullptr;
    QLabel *m_lastUpdatedValueLabel = nullptr;
    QLineEdit *m_projectNameEdit = nullptr;
    QLineEdit *m_projectRootEdit = nullptr;
    QLineEdit *m_buildDirectoryEdit = nullptr;
    QLineEdit *m_versionEdit = nullptr;
    QLineEdit *m_targetsEdit = nullptr;
    QPlainTextEdit *m_changelogEdit = nullptr;
    QPushButton *m_newProjectButton = nullptr;
    QPushButton *m_buildFolderButton = nullptr;
    QPushButton *m_saveButton = nullptr;
    QPushButton *m_deleteButton = nullptr;
};
