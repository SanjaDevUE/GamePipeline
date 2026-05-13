#include "ui/MainWindow.h"

#include <QDateTime>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>

namespace {
QLabel *createSectionTitle(const QString &text)
{
    auto *label = new QLabel(text);
    label->setObjectName("sectionTitle");
    return label;
}

QFrame *createSeparator()
{
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setObjectName("separator");
    return separator;
}

QString formatDateTime(const QDateTime &dateTime)
{
    return dateTime.toLocalTime().toString("yyyy-MM-dd HH:mm");
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    seedProjects();
    setupUi();
    refreshProjectList();

    if (!m_projects.isEmpty()) {
        m_projectList->setCurrentRow(0);
    }
}

void MainWindow::setupUi()
{
    setWindowTitle("GamePipeline");
    resize(1180, 760);
    setMinimumSize(980, 620);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(createSidebar());
    splitter->addWidget(createProjectPanel());
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({280, 900});

    setCentralWidget(splitter);
    statusBar()->showMessage("Ready");
}

QWidget *MainWindow::createSidebar()
{
    auto *sidebar = new QWidget;
    sidebar->setObjectName("sidebar");
    sidebar->setMinimumWidth(250);

    auto *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(20, 22, 20, 20);
    layout->setSpacing(14);

    auto *title = new QLabel("GamePipeline");
    title->setObjectName("appTitle");
    layout->addWidget(title);

    auto *subtitle = new QLabel("Projects");
    subtitle->setObjectName("sidebarSection");
    layout->addWidget(subtitle);

    m_projectList = new QListWidget;
    m_projectList->setObjectName("projectList");
    m_projectList->setAlternatingRowColors(false);
    connect(m_projectList, &QListWidget::currentRowChanged, this, &MainWindow::showProject);
    layout->addWidget(m_projectList, 1);

    auto *newProjectButton = createActionButton("New Project", "primaryButton");
    auto *openProjectButton = createActionButton("Open Folder");
    layout->addWidget(newProjectButton);
    layout->addWidget(openProjectButton);

    connect(newProjectButton, &QPushButton::clicked, this, [this] {
        appendLog("New project action queued.");
    });
    connect(openProjectButton, &QPushButton::clicked, this, [this] {
        appendLog("Open folder action queued.");
    });

    return sidebar;
}

QWidget *MainWindow::createProjectPanel()
{
    auto *panel = new QWidget;
    panel->setObjectName("content");

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(26, 24, 26, 24);
    layout->setSpacing(18);

    auto *headerLayout = new QHBoxLayout;
    headerLayout->setSpacing(12);

    auto *heading = new QLabel("Project Workspace");
    heading->setObjectName("pageTitle");
    headerLayout->addWidget(heading, 1);

    auto *selectBuildButton = createActionButton("Build Folder");
    auto *steamUploadButton = createActionButton("Steam Upload", "primaryButton");
    auto *itchUploadButton = createActionButton("itch.io Upload", "accentButton");

    headerLayout->addWidget(selectBuildButton);
    headerLayout->addWidget(steamUploadButton);
    headerLayout->addWidget(itchUploadButton);
    layout->addLayout(headerLayout);

    connect(selectBuildButton, &QPushButton::clicked, this, [this] {
        appendLog("Build folder selection queued.");
    });
    connect(steamUploadButton, &QPushButton::clicked, this, [this] {
        appendLog("Steam upload action queued.");
    });
    connect(itchUploadButton, &QPushButton::clicked, this, [this] {
        appendLog("itch.io upload action queued.");
    });

    auto *contentLayout = new QHBoxLayout;
    contentLayout->setSpacing(18);
    contentLayout->addWidget(createDetailsPanel(), 2);
    contentLayout->addWidget(createLogPanel(), 1);
    layout->addLayout(contentLayout, 1);

    return panel;
}

QWidget *MainWindow::createDetailsPanel()
{
    auto *panel = new QWidget;
    panel->setObjectName("detailsPanel");

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(22, 22, 22, 22);
    layout->setSpacing(12);

    layout->addWidget(createSectionTitle("Overview"));

    m_projectNameLabel = new QLabel;
    m_projectNameLabel->setObjectName("projectName");
    layout->addWidget(m_projectNameLabel);

    m_projectRootLabel = new QLabel;
    m_buildDirectoryLabel = new QLabel;
    m_versionLabel = new QLabel;
    m_targetsLabel = new QLabel;
    m_lastUpdatedLabel = new QLabel;

    for (auto *label : {m_projectRootLabel, m_buildDirectoryLabel, m_versionLabel, m_targetsLabel, m_lastUpdatedLabel}) {
        label->setObjectName("metadata");
        label->setWordWrap(true);
        layout->addWidget(label);
    }

    layout->addWidget(createSeparator());
    layout->addWidget(createSectionTitle("Changelog"));

    m_changelogOutput = new QPlainTextEdit;
    m_changelogOutput->setObjectName("changelogOutput");
    m_changelogOutput->setReadOnly(true);
    m_changelogOutput->setMinimumHeight(160);
    layout->addWidget(m_changelogOutput);

    layout->addWidget(createSeparator());
    layout->addWidget(createSectionTitle("History"));

    auto *history = new QLabel("No uploads have been recorded for this session.");
    history->setObjectName("metadata");
    history->setWordWrap(true);
    layout->addWidget(history);
    layout->addStretch(1);

    return panel;
}

QWidget *MainWindow::createLogPanel()
{
    auto *panel = new QWidget;
    panel->setObjectName("logPanel");

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    layout->addWidget(createSectionTitle("Log"));

    m_logOutput = new QPlainTextEdit;
    m_logOutput->setObjectName("logOutput");
    m_logOutput->setReadOnly(true);
    m_logOutput->setMinimumWidth(330);
    layout->addWidget(m_logOutput, 1);

    return panel;
}

QPushButton *MainWindow::createActionButton(const QString &text, const QString &objectName)
{
    auto *button = new QPushButton(text);
    button->setCursor(Qt::PointingHandCursor);
    if (!objectName.isEmpty()) {
        button->setObjectName(objectName);
    }
    return button;
}

void MainWindow::seedProjects()
{
    const auto now = QDateTime::currentDateTime();
    m_projects = {
        GameProject("Starfall Tactics",
                    "D:/Games/StarfallTactics",
                    "D:/Games/StarfallTactics/Builds/Windows",
                    "0.8.2",
                    "- Rebalanced enemy wave timing\n- Added controller glyphs\n- Fixed save-game migration",
                    {"Steam", "itch.io"},
                    now.addDays(-1)),
        GameProject("Neon Harbor",
                    "D:/Games/NeonHarbor",
                    "D:/Games/NeonHarbor/dist/win64",
                    "1.1.0",
                    "- New arena map\n- Updated localization files\n- Improved startup checks",
                    {"itch.io"},
                    now.addDays(-4)),
    };
}

void MainWindow::refreshProjectList()
{
    m_projectList->clear();
    for (const auto &project : m_projects) {
        m_projectList->addItem(project.name());
    }
}

void MainWindow::showProject(int row)
{
    if (row < 0 || row >= m_projects.size()) {
        return;
    }

    const auto &project = m_projects.at(row);
    m_projectNameLabel->setText(project.name());
    m_projectRootLabel->setText("Project: " + project.projectRoot());
    m_buildDirectoryLabel->setText("Build: " + project.buildDirectory());
    m_versionLabel->setText("Version: " + project.version());
    m_targetsLabel->setText("Targets: " + project.uploadTargets().join(", "));
    m_lastUpdatedLabel->setText("Updated: " + formatDateTime(project.lastUpdated()));
    m_changelogOutput->setPlainText(project.changelog());
    statusBar()->showMessage(project.name());

    appendLog("Selected project: " + project.name());
}

void MainWindow::appendLog(const QString &message)
{
    if (!m_logOutput) {
        return;
    }

    const auto timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    m_logOutput->appendPlainText(QStringLiteral("[%1] %2").arg(timestamp, message));
}
