#pragma once

#include "domain/GameProject.h"
#include "localization/Language.h"

#include <QVector>
#include <QWidget>

class QLabel;
class QListWidget;
class QPlainTextEdit;
class QPushButton;

class ProjectsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectsPage(QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

signals:
    void logRequested(const QString &message);

private:
    void seedProjects();
    void refreshProjectList();
    void showProject(int row);
    void updateProjectDetails(int row, bool writeSelectionLog);
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    QVector<GameProject> m_projects;
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
    QPlainTextEdit *m_changelogOutput = nullptr;
    QPushButton *m_newProjectButton = nullptr;
    QPushButton *m_buildFolderButton = nullptr;
    QPushButton *m_saveButton = nullptr;
};
