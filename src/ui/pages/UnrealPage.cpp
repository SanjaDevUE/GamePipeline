#include "ui/pages/UnrealPage.h"

#include "domain/GameProject.h"
#include "domain/ProjectRepository.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QProcess>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStyle>
#include <QUrl>
#include <QVBoxLayout>

namespace l10n = localization;

namespace {
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

QLabel *createSectionTitle()
{
    auto *label = new QLabel;
    label->setObjectName("sectionTitle");
    return label;
}

QLabel *createFieldLabel()
{
    auto *label = new QLabel;
    label->setObjectName("fieldLabel");
    return label;
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

QString runUatFileName()
{
#ifdef Q_OS_WIN
    return QStringLiteral("RunUAT.bat");
#else
    return QStringLiteral("RunUAT.sh");
#endif
}

QString runUatPath(const QString &enginePath)
{
    const auto resolvedPath = resolvePortablePath(enginePath);
    const QFileInfo fileInfo(resolvedPath);
    if (fileInfo.isFile()) {
        return QDir::toNativeSeparators(fileInfo.absoluteFilePath());
    }

    const QDir engineDirectory(resolvedPath);
    const auto fromInstallRoot = engineDirectory.filePath(QStringLiteral("Engine/Build/BatchFiles/%1").arg(runUatFileName()));
    if (QFileInfo::exists(fromInstallRoot)) {
        return QDir::toNativeSeparators(QFileInfo(fromInstallRoot).absoluteFilePath());
    }

    const auto fromEngineRoot = engineDirectory.filePath(QStringLiteral("Build/BatchFiles/%1").arg(runUatFileName()));
    if (QFileInfo::exists(fromEngineRoot)) {
        return QDir::toNativeSeparators(QFileInfo(fromEngineRoot).absoluteFilePath());
    }

    return QDir::toNativeSeparators(QFileInfo(fromInstallRoot).absoluteFilePath());
}

QString findUprojectFile(const QString &projectRoot)
{
    if (projectRoot.trimmed().isEmpty()) {
        return {};
    }

    QDir directory(resolvePortablePath(projectRoot));
    const auto files = directory.entryInfoList({QStringLiteral("*.uproject")},
                                               QDir::Files,
                                               QDir::Name | QDir::IgnoreCase);
    if (files.isEmpty()) {
        return {};
    }

    return QDir::toNativeSeparators(files.first().absoluteFilePath());
}

QString defaultArchiveDirectory(const GameProject &project)
{
    if (!project.unrealArchiveDirectory().trimmed().isEmpty()) {
        return resolvePortablePath(project.unrealArchiveDirectory());
    }
    if (!project.buildDirectory().trimmed().isEmpty()) {
        return resolvePortablePath(project.buildDirectory());
    }
    if (!project.projectRoot().trimmed().isEmpty()) {
        return QDir::toNativeSeparators(QDir(resolvePortablePath(project.projectRoot())).filePath(QStringLiteral("Builds")));
    }

    return {};
}

void setComboText(QComboBox *comboBox, const QString &value, const QString &fallback)
{
    const auto text = value.trimmed().isEmpty() ? fallback : value.trimmed();
    const auto index = comboBox->findText(text, Qt::MatchFixedString);
    if (index >= 0) {
        comboBox->setCurrentIndex(index);
        return;
    }

    if (comboBox->isEditable()) {
        comboBox->setEditText(text);
        return;
    }

    comboBox->addItem(text);
    comboBox->setCurrentIndex(comboBox->count() - 1);
}
}

UnrealPage::UnrealPage(ProjectRepository *repository, QWidget *parent)
    : QWidget(parent),
      m_repository(repository)
{
    setObjectName("page");
    setMinimumHeight(680);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(34, 28, 34, 32);
    layout->setSpacing(22);

    auto *headerLayout = new QHBoxLayout;
    headerLayout->setSpacing(12);
    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("pageTitle");
    headerLayout->addWidget(m_titleLabel, 1);

    m_projectLabel = new QLabel;
    m_projectComboBox = new QComboBox;
    m_projectComboBox->setMinimumWidth(240);
    m_saveProjectButton = createActionButton({});
    headerLayout->addWidget(m_projectLabel);
    headerLayout->addWidget(m_projectComboBox);
    headerLayout->addWidget(m_saveProjectButton);
    layout->addLayout(headerLayout);

    auto *pathsPanel = new QWidget;
    pathsPanel->setObjectName("panel");
    auto *pathsLayout = new QVBoxLayout(pathsPanel);
    pathsLayout->setContentsMargins(20, 20, 20, 20);
    pathsLayout->setSpacing(14);
    m_pathsSectionLabel = createSectionTitle();
    pathsLayout->addWidget(m_pathsSectionLabel);

    auto *pathsGrid = new QGridLayout;
    pathsGrid->setContentsMargins(0, 0, 0, 0);
    pathsGrid->setHorizontalSpacing(14);
    pathsGrid->setVerticalSpacing(8);
    pathsGrid->setColumnStretch(0, 1);
    pathsGrid->setColumnStretch(1, 1);
    pathsGrid->setColumnStretch(2, 1);

    m_enginePathLabel = createFieldLabel();
    m_projectPathLabel = createFieldLabel();
    m_archiveDirectoryLabel = createFieldLabel();
    m_enginePathEdit = createInput(QStringLiteral("C:/Program Files/Epic Games/UE_5.4"));
    m_projectPathEdit = createInput(QStringLiteral("D:/Games/MyGame/MyGame.uproject"));
    m_archiveDirectoryEdit = createInput(QStringLiteral("D:/Games/MyGame/Builds/Windows"));
    m_browseEngineButton = createActionButton({});
    m_browseProjectButton = createActionButton({});
    m_browseArchiveButton = createActionButton({});

    auto *engineRow = new QHBoxLayout;
    engineRow->setSpacing(10);
    engineRow->addWidget(m_enginePathEdit, 1);
    engineRow->addWidget(m_browseEngineButton);

    auto *projectRow = new QHBoxLayout;
    projectRow->setSpacing(10);
    projectRow->addWidget(m_projectPathEdit, 1);
    projectRow->addWidget(m_browseProjectButton);

    auto *archiveRow = new QHBoxLayout;
    archiveRow->setSpacing(10);
    archiveRow->addWidget(m_archiveDirectoryEdit, 1);
    archiveRow->addWidget(m_browseArchiveButton);

    pathsGrid->addWidget(m_enginePathLabel, 0, 0);
    pathsGrid->addWidget(m_projectPathLabel, 0, 1);
    pathsGrid->addWidget(m_archiveDirectoryLabel, 0, 2);
    pathsGrid->addLayout(engineRow, 1, 0);
    pathsGrid->addLayout(projectRow, 1, 1);
    pathsGrid->addLayout(archiveRow, 1, 2);
    pathsLayout->addLayout(pathsGrid);
    layout->addWidget(pathsPanel);

    auto *buildPanel = new QWidget;
    buildPanel->setObjectName("panel");
    buildPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    auto *buildLayout = new QVBoxLayout(buildPanel);
    buildLayout->setContentsMargins(20, 20, 20, 20);
    buildLayout->setSpacing(14);
    m_buildSectionLabel = createSectionTitle();
    buildLayout->addWidget(m_buildSectionLabel);

    auto *buildGrid = new QGridLayout;
    buildGrid->setContentsMargins(0, 0, 0, 0);
    buildGrid->setHorizontalSpacing(14);
    buildGrid->setVerticalSpacing(8);
    buildGrid->setColumnStretch(2, 1);

    m_platformLabel = createFieldLabel();
    m_configurationLabel = createFieldLabel();
    m_additionalArgumentsLabel = createFieldLabel();
    m_platformComboBox = new QComboBox;
    m_platformComboBox->setEditable(true);
    m_platformComboBox->addItems({QStringLiteral("Win64"),
                                  QStringLiteral("Linux"),
                                  QStringLiteral("Mac"),
                                  QStringLiteral("Android"),
                                  QStringLiteral("IOS")});
    m_configurationComboBox = new QComboBox;
    m_configurationComboBox->addItems({QStringLiteral("Development"),
                                       QStringLiteral("Shipping"),
                                       QStringLiteral("DebugGame"),
                                       QStringLiteral("Test")});
    m_additionalArgumentsEdit = createInput(QStringLiteral("-utf8output"));
    buildGrid->addWidget(m_platformLabel, 0, 0);
    buildGrid->addWidget(m_configurationLabel, 0, 1);
    buildGrid->addWidget(m_additionalArgumentsLabel, 0, 2);
    buildGrid->addWidget(m_platformComboBox, 1, 0);
    buildGrid->addWidget(m_configurationComboBox, 1, 1);
    buildGrid->addWidget(m_additionalArgumentsEdit, 1, 2);
    buildLayout->addLayout(buildGrid);

    auto *optionLayout = new QHBoxLayout;
    optionLayout->setSpacing(12);
    m_buildCheckBox = new QCheckBox;
    m_cookCheckBox = new QCheckBox;
    m_stageCheckBox = new QCheckBox;
    m_pakCheckBox = new QCheckBox;
    m_archiveCheckBox = new QCheckBox;
    m_cleanCheckBox = new QCheckBox;
    m_distributionCheckBox = new QCheckBox;
    optionLayout->addWidget(m_buildCheckBox);
    optionLayout->addWidget(m_cookCheckBox);
    optionLayout->addWidget(m_stageCheckBox);
    optionLayout->addWidget(m_pakCheckBox);
    optionLayout->addWidget(m_archiveCheckBox);
    optionLayout->addWidget(m_cleanCheckBox);
    optionLayout->addWidget(m_distributionCheckBox);
    optionLayout->addStretch(1);
    buildLayout->addLayout(optionLayout);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(12);
    m_validateButton = createActionButton({});
    m_packageButton = createActionButton({}, "primaryButton");
    m_openArchiveButton = createActionButton({});
    m_validateButton->setMinimumHeight(42);
    m_packageButton->setMinimumHeight(42);
    m_openArchiveButton->setMinimumHeight(42);
    buttonLayout->addWidget(m_validateButton);
    buttonLayout->addWidget(m_packageButton);
    buttonLayout->addWidget(m_openArchiveButton);
    buttonLayout->addStretch(1);
    buildLayout->addLayout(buttonLayout);

    m_progressBar = new QProgressBar;
    m_progressBar->setObjectName("unrealProgressBar");
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setMinimumHeight(32);
    buildLayout->addWidget(m_progressBar);
    layout->addWidget(buildPanel);
    layout->addStretch(1);

    m_packageProcess = new QProcess(this);

    connect(m_projectComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UnrealPage::loadProject);
    connect(m_saveProjectButton, &QPushButton::clicked, this, &UnrealPage::saveCurrentProject);
    connect(m_browseEngineButton, &QPushButton::clicked, this, &UnrealPage::browseEnginePath);
    connect(m_browseProjectButton, &QPushButton::clicked, this, &UnrealPage::browseProjectPath);
    connect(m_browseArchiveButton, &QPushButton::clicked, this, &UnrealPage::browseArchiveDirectory);
    connect(m_validateButton, &QPushButton::clicked, this, [this] {
        validateConfiguration();
    });
    connect(m_packageButton, &QPushButton::clicked, this, &UnrealPage::packageProject);
    connect(m_openArchiveButton, &QPushButton::clicked, this, &UnrealPage::openArchiveDirectory);
    connect(m_packageProcess, &QProcess::readyReadStandardOutput, this, &UnrealPage::appendProcessOutput);
    connect(m_packageProcess, &QProcess::readyReadStandardError, this, &UnrealPage::appendProcessOutput);
    connect(m_packageProcess, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        appendPackageLog(QStringLiteral("RunUAT finished with exit code %1.").arg(exitCode));
        if (exitCode == 0) {
            setProgress(100, l10n::translate(m_language, l10n::Text::UnrealProgressComplete), QStringLiteral("complete"));
            emit logRequested(l10n::translate(m_language, l10n::Text::LogUnrealPackageFinished));
        } else {
            setProgressError(l10n::translate(m_language, l10n::Text::UnrealProgressFailed));
            emit logRequested(l10n::translate(m_language, l10n::Text::LogUnrealPackageFailed)
                                  .arg(QStringLiteral("exit code %1").arg(exitCode)));
        }
    });
    connect(m_packageProcess, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
        const auto message = m_packageProcess->errorString();
        appendPackageLog(message);
        setProgressError(l10n::translate(m_language, l10n::Text::UnrealProgressFailed));
        emit logRequested(l10n::translate(m_language, l10n::Text::LogUnrealPackageFailed).arg(message));
    });

    if (m_repository) {
        connect(m_repository, &ProjectRepository::projectsChanged, this, [this] {
            refreshProjects(currentProjectRow());
        });
    }

    updateTexts();
    setProgress(0, l10n::translate(m_language, l10n::Text::UnrealProgressReady));
    refreshProjects();
}

void UnrealPage::refreshProjects(int preferredRow)
{
    if (!m_repository) {
        clearUnrealForm();
        return;
    }

    const auto projectCount = m_repository->projects().size();
    auto targetRow = preferredRow >= 0 ? preferredRow : currentProjectRow();
    if (targetRow >= projectCount) {
        targetRow = projectCount - 1;
    }
    if (targetRow < 0 && projectCount > 0) {
        targetRow = 0;
    }

    {
        const QSignalBlocker blocker(m_projectComboBox);
        m_projectComboBox->clear();
        if (projectCount == 0) {
            m_projectComboBox->addItem(QStringLiteral("New Unreal Project"), -1);
            m_projectComboBox->setCurrentIndex(0);
        }
        for (int i = 0; i < projectCount; ++i) {
            m_projectComboBox->addItem(m_repository->projects().at(i).name(), i);
        }

        if (targetRow >= 0) {
            m_projectComboBox->setCurrentIndex(targetRow);
        }
    }

    m_projectComboBox->setEnabled(projectCount > 0);
    if (targetRow >= 0) {
        loadProject(targetRow);
    } else {
        prepareDraftProject();
    }
}

void UnrealPage::loadProject(int comboIndex)
{
    Q_UNUSED(comboIndex)

    const auto row = currentProjectRow();
    if (!m_repository || row < 0 || row >= m_repository->projects().size()) {
        prepareDraftProject();
        return;
    }

    const auto &project = m_repository->projects().at(row);
    setUnrealFormEnabled(true);
    m_enginePathEdit->setText(project.unrealEnginePath().isEmpty()
                                  ? QString{}
                                  : resolvePortablePath(project.unrealEnginePath()));
    m_projectPathEdit->setText(project.unrealProjectPath().isEmpty()
                                   ? findUprojectFile(project.projectRoot())
                                   : resolvePortablePath(project.unrealProjectPath()));
    m_archiveDirectoryEdit->setText(defaultArchiveDirectory(project));
    setComboText(m_platformComboBox, project.unrealPlatform(), QStringLiteral("Win64"));
    setComboText(m_configurationComboBox, project.unrealConfiguration(), QStringLiteral("Development"));
    m_additionalArgumentsEdit->setText(project.unrealAdditionalArguments());
    m_buildCheckBox->setChecked(project.unrealBuild());
    m_cookCheckBox->setChecked(project.unrealCook());
    m_stageCheckBox->setChecked(project.unrealStage());
    m_pakCheckBox->setChecked(project.unrealPak());
    m_archiveCheckBox->setChecked(project.unrealArchive());
    m_cleanCheckBox->setChecked(project.unrealClean());
    m_distributionCheckBox->setChecked(project.unrealDistribution());
    setProgress(0, l10n::translate(m_language, l10n::Text::UnrealProgressReady));
}

void UnrealPage::prepareDraftProject()
{
    setUnrealFormEnabled(true);
    m_enginePathEdit->clear();
    m_projectPathEdit->clear();
    m_archiveDirectoryEdit->clear();
    setComboText(m_platformComboBox, QString{}, QStringLiteral("Win64"));
    setComboText(m_configurationComboBox, QString{}, QStringLiteral("Development"));
    m_additionalArgumentsEdit->setText(QStringLiteral("-utf8output"));
    m_buildCheckBox->setChecked(true);
    m_cookCheckBox->setChecked(true);
    m_stageCheckBox->setChecked(true);
    m_pakCheckBox->setChecked(true);
    m_archiveCheckBox->setChecked(true);
    m_cleanCheckBox->setChecked(false);
    m_distributionCheckBox->setChecked(false);
    setProgress(0, l10n::translate(m_language, l10n::Text::UnrealProgressReady));
}

void UnrealPage::clearUnrealForm()
{
    m_enginePathEdit->clear();
    m_projectPathEdit->clear();
    m_archiveDirectoryEdit->clear();
    m_additionalArgumentsEdit->clear();
    setComboText(m_platformComboBox, QString{}, QStringLiteral("Win64"));
    setComboText(m_configurationComboBox, QString{}, QStringLiteral("Development"));
    m_buildCheckBox->setChecked(false);
    m_cookCheckBox->setChecked(false);
    m_stageCheckBox->setChecked(false);
    m_pakCheckBox->setChecked(false);
    m_archiveCheckBox->setChecked(false);
    m_cleanCheckBox->setChecked(false);
    m_distributionCheckBox->setChecked(false);
    setProgress(0, l10n::translate(m_language, l10n::Text::UnrealProgressReady));
    setUnrealFormEnabled(false);
}

void UnrealPage::setUnrealFormEnabled(bool enabled)
{
    m_saveProjectButton->setEnabled(enabled);
    m_enginePathEdit->setEnabled(enabled);
    m_projectPathEdit->setEnabled(enabled);
    m_archiveDirectoryEdit->setEnabled(enabled);
    m_additionalArgumentsEdit->setEnabled(enabled);
    m_platformComboBox->setEnabled(enabled);
    m_configurationComboBox->setEnabled(enabled);
    m_buildCheckBox->setEnabled(enabled);
    m_cookCheckBox->setEnabled(enabled);
    m_stageCheckBox->setEnabled(enabled);
    m_pakCheckBox->setEnabled(enabled);
    m_archiveCheckBox->setEnabled(enabled);
    m_cleanCheckBox->setEnabled(enabled);
    m_distributionCheckBox->setEnabled(enabled);
    m_browseEngineButton->setEnabled(enabled);
    m_browseProjectButton->setEnabled(enabled);
    m_browseArchiveButton->setEnabled(enabled);
    m_validateButton->setEnabled(enabled);
    m_packageButton->setEnabled(enabled);
    m_openArchiveButton->setEnabled(enabled);
}

void UnrealPage::saveCurrentProject()
{
    setProgress(30, l10n::translate(m_language, l10n::Text::UnrealProgressSaving));
    if (persistCurrentProject(true)) {
        setProgress(35, l10n::translate(m_language, l10n::Text::UnrealProgressSaved));
    }
}

void UnrealPage::browseEnginePath()
{
    const auto folder = QFileDialog::getExistingDirectory(this,
                                                          l10n::translate(m_language, l10n::Text::UnrealEnginePath),
                                                          resolvePortablePath(m_enginePathEdit->text()));
    if (folder.isEmpty()) {
        return;
    }

    m_enginePathEdit->setText(QDir::toNativeSeparators(folder));
}

void UnrealPage::browseProjectPath()
{
    const auto file = QFileDialog::getOpenFileName(this,
                                                   l10n::translate(m_language, l10n::Text::UnrealProjectFile),
                                                   resolvePortablePath(m_projectPathEdit->text()),
                                                   QStringLiteral("Unreal projects (*.uproject)"));
    if (file.isEmpty()) {
        return;
    }

    m_projectPathEdit->setText(QDir::toNativeSeparators(file));
    if (m_archiveDirectoryEdit->text().trimmed().isEmpty()) {
        m_archiveDirectoryEdit->setText(QDir::toNativeSeparators(QFileInfo(file).absoluteDir().filePath(QStringLiteral("Builds"))));
    }
}

void UnrealPage::browseArchiveDirectory()
{
    const auto folder = QFileDialog::getExistingDirectory(this,
                                                          l10n::translate(m_language, l10n::Text::UnrealArchiveDirectory),
                                                          resolvePortablePath(m_archiveDirectoryEdit->text()));
    if (folder.isEmpty()) {
        return;
    }

    m_archiveDirectoryEdit->setText(QDir::toNativeSeparators(folder));
}

bool UnrealPage::persistCurrentProject(bool writeLog)
{
    if (!m_repository) {
        return false;
    }

    const auto row = currentProjectRow();
    const bool updateExistingProject = row >= 0 && row < m_repository->projects().size();
    const auto projectPath = resolvePortablePath(m_projectPathEdit->text());
    const QFileInfo projectFile(projectPath);
    const auto projectName = projectFile.completeBaseName().isEmpty()
                                 ? QStringLiteral("Unreal Project")
                                 : projectFile.completeBaseName();

    auto project = updateExistingProject
                       ? m_repository->projects().at(row)
                       : GameProject(projectName,
                                     projectFile.absolutePath(),
                                     resolvePortablePath(m_archiveDirectoryEdit->text()),
                                     QStringLiteral("0.1.0"),
                                     {},
                                     {QStringLiteral("Unreal")},
                                     QDateTime::currentDateTime());

    project.setUnrealEnginePath(makePortablePath(m_enginePathEdit->text()));
    project.setUnrealProjectPath(makePortablePath(m_projectPathEdit->text()));
    project.setUnrealArchiveDirectory(makePortablePath(m_archiveDirectoryEdit->text()));
    project.setUnrealPlatform(m_platformComboBox->currentText().trimmed());
    project.setUnrealConfiguration(m_configurationComboBox->currentText().trimmed());
    project.setUnrealAdditionalArguments(m_additionalArgumentsEdit->text().trimmed());
    project.setUnrealBuild(m_buildCheckBox->isChecked());
    project.setUnrealCook(m_cookCheckBox->isChecked());
    project.setUnrealStage(m_stageCheckBox->isChecked());
    project.setUnrealPak(m_pakCheckBox->isChecked());
    project.setUnrealArchive(m_archiveCheckBox->isChecked());
    project.setUnrealClean(m_cleanCheckBox->isChecked());
    project.setUnrealDistribution(m_distributionCheckBox->isChecked());
    if (project.projectRoot().trimmed().isEmpty() && projectFile.exists()) {
        project.setProjectRoot(makePortablePath(projectFile.absolutePath()));
    }
    if (!m_archiveDirectoryEdit->text().trimmed().isEmpty()) {
        project.setBuildDirectory(makePortablePath(m_archiveDirectoryEdit->text()));
    }
    project.setLastUpdated(QDateTime::currentDateTime());

    auto targets = project.uploadTargets();
    if (!targets.contains(QStringLiteral("Unreal"), Qt::CaseInsensitive)) {
        targets.append(QStringLiteral("Unreal"));
        project.setUploadTargets(targets);
    }

    const auto savedRow = updateExistingProject ? row : m_repository->addProject(project);
    if (updateExistingProject) {
        m_repository->updateProject(row, project);
    }
    refreshProjects(savedRow);
    if (writeLog) {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogUnrealProjectSaved).arg(project.name()));
    }
    return true;
}

bool UnrealPage::validateConfiguration()
{
    setProgress(10, l10n::translate(m_language, l10n::Text::UnrealProgressValidating));

    QStringList missing;
    if (m_enginePathEdit->text().trimmed().isEmpty()) {
        missing.append(l10n::translate(m_language, l10n::Text::UnrealEnginePath));
    }
    if (m_projectPathEdit->text().trimmed().isEmpty()) {
        missing.append(l10n::translate(m_language, l10n::Text::UnrealProjectFile));
    }
    if (m_platformComboBox->currentText().trimmed().isEmpty()) {
        missing.append(l10n::translate(m_language, l10n::Text::UnrealPlatform));
    }
    if (m_configurationComboBox->currentText().trimmed().isEmpty()) {
        missing.append(l10n::translate(m_language, l10n::Text::UnrealBuildConfiguration));
    }
    if (m_archiveCheckBox->isChecked() && m_archiveDirectoryEdit->text().trimmed().isEmpty()) {
        missing.append(l10n::translate(m_language, l10n::Text::UnrealArchiveDirectory));
    }

    if (!missing.isEmpty()) {
        appendPackageLog(QStringLiteral("Missing configuration: %1").arg(missing.join(QStringLiteral(", "))));
        setProgressError(l10n::translate(m_language, l10n::Text::UnrealProgressFailed));
        return false;
    }

    const auto executablePath = runUatPath(m_enginePathEdit->text());
    if (!QFileInfo::exists(executablePath)) {
        appendPackageLog(QStringLiteral("RunUAT not found at %1").arg(QDir::toNativeSeparators(executablePath)));
        setProgressError(l10n::translate(m_language, l10n::Text::UnrealProgressFailed));
        return false;
    }

    const auto projectPath = resolvePortablePath(m_projectPathEdit->text());
    if (!QFileInfo::exists(projectPath) || QFileInfo(projectPath).suffix().compare(QStringLiteral("uproject"), Qt::CaseInsensitive) != 0) {
        appendPackageLog(QStringLiteral("Unreal project file is invalid: %1").arg(projectPath));
        setProgressError(l10n::translate(m_language, l10n::Text::UnrealProgressFailed));
        return false;
    }

    appendPackageLog(QStringLiteral("Unreal packaging configuration is complete."));
    setProgress(25, l10n::translate(m_language, l10n::Text::UnrealProgressValidated));
    emit logRequested(l10n::translate(m_language, l10n::Text::LogUnrealValidate));
    return true;
}

void UnrealPage::packageProject()
{
    if (!validateConfiguration()) {
        return;
    }
    if (m_packageProcess->state() != QProcess::NotRunning) {
        appendPackageLog(QStringLiteral("RunUAT is already running."));
        return;
    }

    setProgress(30, l10n::translate(m_language, l10n::Text::UnrealProgressSaving));
    if (!persistCurrentProject(false)) {
        return;
    }

    const auto archiveDirectory = resolvePortablePath(m_archiveDirectoryEdit->text());
    if (m_archiveCheckBox->isChecked() && !QDir().mkpath(archiveDirectory)) {
        appendPackageLog(QStringLiteral("Could not create archive directory: %1").arg(archiveDirectory));
        setProgressError(l10n::translate(m_language, l10n::Text::UnrealProgressFailed));
        return;
    }

    const auto executablePath = runUatPath(m_enginePathEdit->text());
    const auto projectPath = resolvePortablePath(m_projectPathEdit->text());
    QStringList arguments;
    arguments << QStringLiteral("BuildCookRun")
              << QStringLiteral("-project=%1").arg(projectPath)
              << QStringLiteral("-noP4")
              << QStringLiteral("-platform=%1").arg(m_platformComboBox->currentText().trimmed())
              << QStringLiteral("-clientconfig=%1").arg(m_configurationComboBox->currentText().trimmed());

    if (m_buildCheckBox->isChecked()) {
        arguments << QStringLiteral("-build");
    }
    if (m_cookCheckBox->isChecked()) {
        arguments << QStringLiteral("-cook");
    }
    if (m_stageCheckBox->isChecked()) {
        arguments << QStringLiteral("-stage");
    }
    if (m_pakCheckBox->isChecked()) {
        arguments << QStringLiteral("-pak");
    }
    if (m_archiveCheckBox->isChecked()) {
        arguments << QStringLiteral("-archive")
                  << QStringLiteral("-archivedirectory=%1").arg(archiveDirectory);
    }
    if (m_cleanCheckBox->isChecked()) {
        arguments << QStringLiteral("-clean");
    }
    if (m_distributionCheckBox->isChecked()) {
        arguments << QStringLiteral("-distribution");
    }

    if (!m_additionalArgumentsEdit->text().trimmed().isEmpty()) {
        arguments << QProcess::splitCommand(m_additionalArgumentsEdit->text().trimmed());
    }

#ifdef Q_OS_WIN
    QStringList processArguments;
    processArguments << QStringLiteral("/C") << executablePath << arguments;
    m_packageProcess->setProgram(QStringLiteral("cmd.exe"));
    m_packageProcess->setArguments(processArguments);
#else
    m_packageProcess->setProgram(executablePath);
    m_packageProcess->setArguments(arguments);
#endif
    m_packageProcess->setWorkingDirectory(QFileInfo(executablePath).absolutePath());
    m_packageProcess->start();
    if (!m_packageProcess->waitForStarted(1500)) {
        const auto message = m_packageProcess->errorString();
        appendPackageLog(message);
        setProgressError(l10n::translate(m_language, l10n::Text::UnrealProgressFailed));
        emit logRequested(l10n::translate(m_language, l10n::Text::LogUnrealPackageFailed).arg(message));
        return;
    }

    appendPackageLog(QStringLiteral("Started RunUAT BuildCookRun."));
    setProgressBusy(l10n::translate(m_language, l10n::Text::UnrealProgressPackaging));
    emit logRequested(l10n::translate(m_language, l10n::Text::LogUnrealPackageStarted));
}

void UnrealPage::openArchiveDirectory()
{
    const auto folder = resolvePortablePath(m_archiveDirectoryEdit->text());
    if (!folder.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
    }
}

void UnrealPage::appendPackageLog(const QString &message)
{
    emit logRequested(QStringLiteral("Unreal: %1").arg(message));
}

void UnrealPage::appendProcessOutput()
{
    const auto output = QString::fromLocal8Bit(m_packageProcess->readAllStandardOutput())
                            + QString::fromLocal8Bit(m_packageProcess->readAllStandardError());
    if (!output.trimmed().isEmpty()) {
        appendPackageLog(output.trimmed());
    }
}

void UnrealPage::setProgress(int value, const QString &text, const QString &state)
{
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(qBound(0, value, 100));
    m_progressBar->setFormat(QStringLiteral("%1 - %p%").arg(text));
    m_progressBar->setProperty("state", state.isEmpty() ? QStringLiteral("active") : state);
    m_progressBar->style()->unpolish(m_progressBar);
    m_progressBar->style()->polish(m_progressBar);
}

void UnrealPage::setProgressBusy(const QString &text)
{
    m_progressBar->setRange(0, 0);
    m_progressBar->setFormat(text);
    m_progressBar->setProperty("state", QStringLiteral("active"));
    m_progressBar->style()->unpolish(m_progressBar);
    m_progressBar->style()->polish(m_progressBar);
}

void UnrealPage::setProgressError(const QString &text)
{
    setProgress(0, text, QStringLiteral("error"));
}

int UnrealPage::currentProjectRow() const
{
    const auto data = m_projectComboBox->currentData();
    return data.isValid() ? data.toInt() : -1;
}

void UnrealPage::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void UnrealPage::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::UnrealTitle));
    m_projectLabel->setText(l10n::translate(m_language, l10n::Text::Project));
    m_saveProjectButton->setText(l10n::translate(m_language, l10n::Text::SaveSettings));
    m_pathsSectionLabel->setText(l10n::translate(m_language, l10n::Text::UnrealConfiguration));
    m_buildSectionLabel->setText(l10n::translate(m_language, l10n::Text::UnrealBuildOptions));
    m_enginePathLabel->setText(l10n::translate(m_language, l10n::Text::UnrealEnginePath));
    m_projectPathLabel->setText(l10n::translate(m_language, l10n::Text::UnrealProjectFile));
    m_archiveDirectoryLabel->setText(l10n::translate(m_language, l10n::Text::UnrealArchiveDirectory));
    m_platformLabel->setText(l10n::translate(m_language, l10n::Text::UnrealPlatform));
    m_configurationLabel->setText(l10n::translate(m_language, l10n::Text::UnrealBuildConfiguration));
    m_additionalArgumentsLabel->setText(l10n::translate(m_language, l10n::Text::UnrealAdditionalArguments));
    m_buildCheckBox->setText(l10n::translate(m_language, l10n::Text::UnrealBuild));
    m_cookCheckBox->setText(l10n::translate(m_language, l10n::Text::UnrealCook));
    m_stageCheckBox->setText(l10n::translate(m_language, l10n::Text::UnrealStage));
    m_pakCheckBox->setText(l10n::translate(m_language, l10n::Text::UnrealPak));
    m_archiveCheckBox->setText(l10n::translate(m_language, l10n::Text::UnrealArchive));
    m_cleanCheckBox->setText(l10n::translate(m_language, l10n::Text::UnrealClean));
    m_distributionCheckBox->setText(l10n::translate(m_language, l10n::Text::UnrealDistribution));
    m_browseEngineButton->setText(l10n::translate(m_language, l10n::Text::Browse));
    m_browseProjectButton->setText(l10n::translate(m_language, l10n::Text::Browse));
    m_browseArchiveButton->setText(l10n::translate(m_language, l10n::Text::Browse));
    m_validateButton->setText(l10n::translate(m_language, l10n::Text::ValidateConfig));
    m_packageButton->setText(l10n::translate(m_language, l10n::Text::UnrealPackageProject));
    m_openArchiveButton->setText(l10n::translate(m_language, l10n::Text::UnrealOpenArchiveFolder));
}
