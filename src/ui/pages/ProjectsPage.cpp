#include "ui/pages/ProjectsPage.h"

#include "domain/ProjectRepository.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>

namespace l10n = localization;

namespace {
QString formatDateTime(const QDateTime &dateTime)
{
    return dateTime.isValid() ? dateTime.toLocalTime().toString("yyyy-MM-dd HH:mm") : QStringLiteral("-");
}

QLabel *createSectionTitle()
{
    auto *label = new QLabel;
    label->setObjectName("sectionTitle");
    return label;
}

QLineEdit *createInput(const QString &placeholder = {})
{
    auto *input = new QLineEdit;
    input->setPlaceholderText(placeholder);
    return input;
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

QString nextProjectName(const QVector<GameProject> &projects)
{
    return QStringLiteral("New Game %1").arg(projects.size() + 1);
}

QStringList portableRootCandidates()
{
    QStringList roots;
    const QStringList basePaths = {
        QDir::currentPath(),
        QCoreApplication::applicationDirPath(),
    };

    auto addRoot = [&roots](const QString &path) {
        const auto root = QDir::cleanPath(QFileInfo(path).absoluteFilePath());
        if (!roots.contains(root, Qt::CaseInsensitive)) {
            roots.append(root);
        }
    };

    for (const auto &basePath : basePaths) {
        QDir directory(basePath);
        for (int depth = 0; depth < 5; ++depth) {
            addRoot(directory.absolutePath());
            if (!directory.cdUp()) {
                break;
            }
        }
    }

    return roots;
}

QString resolvePortablePath(const QString &path)
{
    const auto trimmedPath = path.trimmed();
    if (trimmedPath.isEmpty()) {
        return {};
    }

    const auto normalizedPath = QDir::fromNativeSeparators(trimmedPath);
    if (QFileInfo(normalizedPath).isAbsolute()) {
        return QDir::toNativeSeparators(QFileInfo(normalizedPath).absoluteFilePath());
    }

    const auto roots = portableRootCandidates();
    for (const auto &root : roots) {
        const auto candidatePath = QDir(root).filePath(normalizedPath);
        if (QFileInfo::exists(candidatePath)) {
            return QDir::toNativeSeparators(QFileInfo(candidatePath).absoluteFilePath());
        }
    }

    return QDir::toNativeSeparators(QFileInfo(QDir(roots.first()).filePath(normalizedPath)).absoluteFilePath());
}

QString makePortablePath(const QString &path)
{
    if (path.trimmed().isEmpty()) {
        return {};
    }

    const auto absolutePath = QFileInfo(resolvePortablePath(path)).absoluteFilePath();
    for (const auto &root : portableRootCandidates()) {
        const auto relativePath = QDir(root).relativeFilePath(absolutePath);
        if (!relativePath.startsWith(QStringLiteral(".."))
            && !QFileInfo(relativePath).isAbsolute()
            && relativePath != QStringLiteral(".")) {
            return QDir::toNativeSeparators(relativePath);
        }
    }

    return QDir::toNativeSeparators(absolutePath);
}
}

ProjectsPage::ProjectsPage(ProjectRepository *repository, QWidget *parent)
    : QWidget(parent),
      m_repository(repository)
{
    setObjectName("page");

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
    m_deleteButton = createActionButton({}, "dangerButton");
    headerLayout->addWidget(m_newProjectButton);
    headerLayout->addWidget(m_buildFolderButton);
    headerLayout->addWidget(m_saveButton);
    headerLayout->addWidget(m_deleteButton);
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

    auto *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);
    form->setHorizontalSpacing(14);
    form->setVerticalSpacing(12);

    m_projectNameLabel = new QLabel;
    m_projectRootLabel = new QLabel;
    m_buildDirectoryLabel = new QLabel;
    m_versionLabel = new QLabel;
    m_targetsLabel = new QLabel;
    m_lastUpdatedLabel = new QLabel;
    m_lastUpdatedValueLabel = new QLabel;
    m_lastUpdatedValueLabel->setObjectName("metadata");

    m_projectNameEdit = createInput("Arcade Shooter");
    m_projectRootEdit = createInput("D:/Games/ArcadeShooter");
    m_buildDirectoryEdit = createInput("D:/Games/ArcadeShooter/Builds/Windows");
    m_versionEdit = createInput("0.1.0");
    m_targetsEdit = createInput("Steam, itch.io");

    form->addRow(m_projectNameLabel, m_projectNameEdit);
    form->addRow(m_projectRootLabel, m_projectRootEdit);
    form->addRow(m_buildDirectoryLabel, m_buildDirectoryEdit);
    form->addRow(m_versionLabel, m_versionEdit);
    form->addRow(m_targetsLabel, m_targetsEdit);
    form->addRow(m_lastUpdatedLabel, m_lastUpdatedValueLabel);
    detailsLayout->addLayout(form);

    m_changelogLabel = createSectionTitle();
    detailsLayout->addWidget(m_changelogLabel);
    m_changelogEdit = new QPlainTextEdit;
    m_changelogEdit->setObjectName("changelogOutput");
    detailsLayout->addWidget(m_changelogEdit, 1);
    contentLayout->addWidget(detailsPanel, 2);

    layout->addLayout(contentLayout, 1);

    connect(m_newProjectButton, &QPushButton::clicked, this, &ProjectsPage::createProject);
    connect(m_buildFolderButton, &QPushButton::clicked, this, &ProjectsPage::selectBuildFolder);
    connect(m_saveButton, &QPushButton::clicked, this, &ProjectsPage::saveCurrentProject);
    connect(m_deleteButton, &QPushButton::clicked, this, &ProjectsPage::deleteCurrentProject);

    if (m_repository) {
        connect(m_repository, &ProjectRepository::projectsChanged, this, [this] {
            refreshProjectList();
        });
    }

    updateTexts();
    refreshProjectList();
}

void ProjectsPage::createProject()
{
    if (!m_repository) {
        return;
    }

    GameProject project(nextProjectName(m_repository->projects()),
                        {},
                        {},
                        QStringLiteral("0.1.0"),
                        {},
                        {QStringLiteral("Steam")},
                        QDateTime::currentDateTime());
    project.setSteamBuildDescription(QStringLiteral("DevBuild"));

    const auto row = m_repository->addProject(project);
    refreshProjectList(row);
    emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsCreated));
}

void ProjectsPage::saveCurrentProject()
{
    if (!m_repository) {
        return;
    }

    const auto row = m_projectList->currentRow();
    if (row < 0 || row >= m_repository->projects().size()) {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsNoProjectSelected));
        return;
    }

    auto project = projectFromForm(m_repository->projects().at(row));
    project.setLastUpdated(QDateTime::currentDateTime());
    m_repository->updateProject(row, project);
    refreshProjectList(row);
    emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsSave).arg(project.name()));
}

void ProjectsPage::deleteCurrentProject()
{
    if (!m_repository) {
        return;
    }

    const auto row = m_projectList->currentRow();
    if (row < 0 || row >= m_repository->projects().size()) {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsNoProjectSelected));
        return;
    }

    const auto projectName = m_repository->projects().at(row).name();
    const auto answer = QMessageBox::question(this,
                                              l10n::translate(m_language, l10n::Text::DeleteProject),
                                              QStringLiteral("Delete \"%1\"?").arg(projectName),
                                              QMessageBox::Yes | QMessageBox::No,
                                              QMessageBox::No);
    if (answer != QMessageBox::Yes) {
        return;
    }

    m_repository->removeProject(row);
    refreshProjectList(qMin(row, m_repository->projects().size() - 1));
    emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsDeleted).arg(projectName));
}

void ProjectsPage::selectBuildFolder()
{
    const auto row = m_projectList->currentRow();
    if (!m_repository || row < 0 || row >= m_repository->projects().size()) {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsNoProjectSelected));
        return;
    }

    const auto folder = QFileDialog::getExistingDirectory(this,
                                                          l10n::translate(m_language, l10n::Text::SelectBuildFolder),
                                                          resolvePortablePath(m_buildDirectoryEdit->text()));
    if (folder.isEmpty()) {
        return;
    }

    m_buildDirectoryEdit->setText(QDir::toNativeSeparators(folder));
    emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsBuildFolder));
}

void ProjectsPage::refreshProjectList(int preferredRow)
{
    if (!m_repository) {
        clearProjectForm();
        return;
    }

    const auto oldRow = m_projectList->currentRow();
    const auto projectCount = m_repository->projects().size();
    auto targetRow = preferredRow >= 0 ? preferredRow : oldRow;
    if (targetRow >= projectCount) {
        targetRow = projectCount - 1;
    }
    if (targetRow < 0 && projectCount > 0) {
        targetRow = 0;
    }

    {
        const QSignalBlocker blocker(m_projectList);
        m_projectList->clear();
        for (const auto &project : m_repository->projects()) {
            m_projectList->addItem(project.name());
        }

        if (targetRow >= 0) {
            m_projectList->setCurrentRow(targetRow);
        }
    }

    if (targetRow >= 0) {
        showProject(targetRow);
    } else {
        clearProjectForm();
    }
}

void ProjectsPage::showProject(int row)
{
    if (!m_repository || row < 0 || row >= m_repository->projects().size()) {
        clearProjectForm();
        return;
    }

    const auto &project = m_repository->projects().at(row);
    setProjectFormEnabled(true);
    m_projectNameEdit->setText(project.name());
    m_projectRootEdit->setText(resolvePortablePath(project.projectRoot()));
    m_buildDirectoryEdit->setText(resolvePortablePath(project.buildDirectory()));
    m_versionEdit->setText(project.version());
    m_targetsEdit->setText(project.uploadTargets().join(QStringLiteral(", ")));
    m_lastUpdatedValueLabel->setText(formatDateTime(project.lastUpdated()));
    m_changelogEdit->setPlainText(project.changelog());

    emit logRequested(l10n::translate(m_language, l10n::Text::LogProjectsSelected).arg(project.name()));
}

void ProjectsPage::clearProjectForm()
{
    setProjectFormEnabled(false);
    m_projectNameEdit->clear();
    m_projectRootEdit->clear();
    m_buildDirectoryEdit->clear();
    m_versionEdit->clear();
    m_targetsEdit->clear();
    m_lastUpdatedValueLabel->setText(l10n::translate(m_language, l10n::Text::NoProjects));
    m_changelogEdit->clear();
}

void ProjectsPage::setProjectFormEnabled(bool enabled)
{
    m_projectNameEdit->setEnabled(enabled);
    m_projectRootEdit->setEnabled(enabled);
    m_buildDirectoryEdit->setEnabled(enabled);
    m_versionEdit->setEnabled(enabled);
    m_targetsEdit->setEnabled(enabled);
    m_changelogEdit->setEnabled(enabled);
    m_buildFolderButton->setEnabled(enabled);
    m_saveButton->setEnabled(enabled);
    m_deleteButton->setEnabled(enabled);
}

GameProject ProjectsPage::projectFromForm(const GameProject &baseProject) const
{
    auto project = baseProject;
    auto name = m_projectNameEdit->text().trimmed();
    if (name.isEmpty()) {
        name = QStringLiteral("Untitled Game");
    }

    project.setName(name);
    project.setProjectRoot(makePortablePath(m_projectRootEdit->text()));
    project.setBuildDirectory(makePortablePath(m_buildDirectoryEdit->text()));
    project.setVersion(m_versionEdit->text().trimmed());
    project.setUploadTargets(uploadTargetsFromText());
    project.setChangelog(m_changelogEdit->toPlainText());
    return project;
}

QStringList ProjectsPage::uploadTargetsFromText() const
{
    QStringList targets;
    const auto parts = m_targetsEdit->text().split(',', Qt::SkipEmptyParts);
    for (const auto &part : parts) {
        const auto target = part.trimmed();
        if (!target.isEmpty()) {
            targets.append(target);
        }
    }

    return targets;
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
    m_deleteButton->setText(l10n::translate(m_language, l10n::Text::DeleteProject));
    m_projectListLabel->setText(l10n::translate(m_language, l10n::Text::ProjectList));
    m_selectedProjectLabel->setText(l10n::translate(m_language, l10n::Text::SelectedProject));
    m_projectNameLabel->setText(l10n::translate(m_language, l10n::Text::ProjectName));
    m_projectRootLabel->setText(l10n::translate(m_language, l10n::Text::ProjectRoot));
    m_buildDirectoryLabel->setText(l10n::translate(m_language, l10n::Text::BuildDirectory));
    m_versionLabel->setText(l10n::translate(m_language, l10n::Text::Version));
    m_targetsLabel->setText(l10n::translate(m_language, l10n::Text::UploadTargets));
    m_lastUpdatedLabel->setText(l10n::translate(m_language, l10n::Text::LastUpdated));
    m_changelogLabel->setText(l10n::translate(m_language, l10n::Text::Changelog));

    if (m_repository && m_projectList->currentRow() < 0) {
        clearProjectForm();
    }
}
