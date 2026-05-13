#pragma once

#include "domain/GameProject.h"

#include <QMainWindow>
#include <QVector>

class QLabel;
class QListWidget;
class QPlainTextEdit;
class QPushButton;
class QWidget;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupUi();
    void seedProjects();
    void refreshProjectList();
    void showProject(int row);
    void appendLog(const QString &message);

    [[nodiscard]] QWidget *createSidebar();
    [[nodiscard]] QWidget *createProjectPanel();
    [[nodiscard]] QWidget *createDetailsPanel();
    [[nodiscard]] QWidget *createLogPanel();
    [[nodiscard]] QPushButton *createActionButton(const QString &text, const QString &objectName = {});

    QVector<GameProject> m_projects;
    QListWidget *m_projectList = nullptr;
    QLabel *m_projectNameLabel = nullptr;
    QLabel *m_projectRootLabel = nullptr;
    QLabel *m_buildDirectoryLabel = nullptr;
    QLabel *m_versionLabel = nullptr;
    QLabel *m_targetsLabel = nullptr;
    QLabel *m_lastUpdatedLabel = nullptr;
    QPlainTextEdit *m_changelogOutput = nullptr;
    QPlainTextEdit *m_logOutput = nullptr;
};
