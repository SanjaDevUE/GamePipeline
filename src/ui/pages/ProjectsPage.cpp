#include "ui/pages/ProjectsPage.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace l10n = localization;

namespace {
QString formatDateTime(const QDateTime &dateTime)
{
    return dateTime.toLocalTime().toString("yyyy-MM-dd HH:mm");
}

QLabel *createSectionTitle()
{
    auto *label = new QLabel;
    label->setObjectName("sectionTitle");
    return label;
}

QLabel *createMetadataLabel()
{
    auto *label = new QLabel;
    label->setObjectName("metadata");
    label->setWordWrap(true);
    return label;
}

QPushButton *createActionButton(const QString &text, const QString &objectName = {})
{
    auto *button = new QPushButton(text);
    button->setCursor(Qt::PointingHandCursor);
    if (!objectName.isEmpty()) {
        button->setObjectName(objectName);
    }
    return button;
}
}

ProjectsPage::ProjectsPage(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("page");
    seedProjects();

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 26, 28, 28);
    layout->setSpacing(18);

    auto *headerLayout = new QHBoxLayout;
    headerLayout->setSpacing(12);

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("pageTitle");
    headerLayout->addWidget(m_titleLabel, 1);

    m_newProjectButton = createActionButton({}, "primaryButton");
    m_buildFolderButton = createActionButton({});
    m_saveButton = createActionButton({});
    headerLayout->addWidget(m_newProjectButton);
    headerLayout->addWidget(m_buildFolderButton);
    headerLayout->addWidget(m_saveButton);
    layout->addLayout(headerLayout);

    auto *contentLayout = new QHBoxLayout;
    contentLayout->setSpacing(18);

    auto *listPanel = new QWidget;
    listPanel->setObjectName("panel");
    auto *listLayout = new QVBoxLayout(listPanel);
    listLayout->setContentsMargins(18, 18, 18, 18);
    listLayout->setSpacing(12);
    m_projectListLabel = createSectionTitle();
    listLayout->addWidget(m_projectListLabel);

    m_projectList = new QListWidget;
    m_projectList->setObjectName("projectList");
    connect(m_projectList, &QListWidget::currentRowChanged, this, &ProjectsPage::showProject);
    listLayout->addWidget(m_projectList, 1);
    contentLayout->addWidget(listPanel, 1);

    auto *detailsPanel = new QWidget;
    detailsPanel->setObjectName("panel");
    auto *detailsLayout = new QVBoxLayout(detailsPanel);
    detailsLayout->setContentsMargins(20, 20, 20, 20);
    detailsLayout->setSpacing(12);
    m_selectedProjectLabel = createSectionTitle();
    detailsLayout->addWidget(m_selectedProjectLabel);

    m_projectNameLabel = new QLabel;
    m_projectNameLabel->setObjectName("projectName");
    detailsLayout->addWidget(m_projectNameLabel);

    m_projectRootLabel = createMetadataLabel();
    m_buildDirectoryLabel = createMetadataLabel();
    m_versionLabel = createMetadataLabel();
    m_targetsLabel = createMetadataLabel();
    m_lastUpdatedLabel = createMetadataLabel();

    detailsLayout->addWidget(m_projectRootLabel);
    detailsLayout->addWidget(m_buildDirectoryLabel);
    detailsLayout->addWidget(m_versionLabel);
    detailsLayout->addWidget(m_targetsLabel);
    detailsLayout->addWidget(m_lastUpdatedLabel);

    m_changelogLabel = createSectionTitle();
    detailsLayout->addWidget(m_changelogLabel);
    m_changelogOutput = new QPlainTextEdit;
    m_changelogOutput->setObjectName("changelogOutput");
    m_changelogOutput->setReadOnly(true);
    detailsLayout->addWidget(m_changelogOutput, 1);
    contentLayout->addWidget(detailsPanel, 2);

    layout->addLayout(contentLayout, 1);

    connect(m_newProjectButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsNewProject));
    });
    connect(m_buildFolderButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsBuildFolder));
    });
    connect(m_saveButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsSave));
    });

    updateTexts();
    refreshProjectList();
    if (!m_projects.isEmpty()) {
        m_projectList->setCurrentRow(0);
    }
}

void ProjectsPage::seedProjects()
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

void ProjectsPage::refreshProjectList()
{
    m_projectList->clear();
    for (const auto &project : m_projects) {
        m_projectList->addItem(project.name());
    }
}

void ProjectsPage::showProject(int row)
{
    updateProjectDetails(row, true);
}

void ProjectsPage::updateProjectDetails(int row, bool writeSelectionLog)
{
    if (row < 0 || row >= m_projects.size()) {
        return;
    }

    const auto &project = m_projects.at(row);
    m_projectNameLabel->setText(project.name());
    m_projectRootLabel->setText(l10n::translate(m_language, l10n::Text::ProjectPrefix).arg(project.projectRoot()));
    m_buildDirectoryLabel->setText(l10n::translate(m_language, l10n::Text::BuildPrefix).arg(project.buildDirectory()));
    m_versionLabel->setText(l10n::translate(m_language, l10n::Text::VersionPrefix).arg(project.version()));
    m_targetsLabel->setText(l10n::translate(m_language, l10n::Text::TargetsPrefix).arg(project.uploadTargets().join(", ")));
    m_lastUpdatedLabel->setText(l10n::translate(m_language, l10n::Text::UpdatedPrefix).arg(formatDateTime(project.lastUpdated())));
    m_changelogOutput->setPlainText(project.changelog());

    if (writeSelectionLog) {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsSelected).arg(project.name()));
    }
}

void ProjectsPage::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void ProjectsPage::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::ProjectsTitle));
    m_newProjectButton->setText(l10n::translate(m_language, l10n::Text::NewProject));
    m_buildFolderButton->setText(l10n::translate(m_language, l10n::Text::SelectBuildFolder));
    m_saveButton->setText(l10n::translate(m_language, l10n::Text::SaveSettings));
    m_projectListLabel->setText(l10n::translate(m_language, l10n::Text::ProjectList));
    m_selectedProjectLabel->setText(l10n::translate(m_language, l10n::Text::SelectedProject));
    m_changelogLabel->setText(l10n::translate(m_language, l10n::Text::Changelog));

    if (m_projectList) {
        updateProjectDetails(m_projectList->currentRow(), false);
    }
}
