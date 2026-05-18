#include "ui/pages/SteamPage.h"

#include "domain/ProjectRepository.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStyle>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QUrl>
#include <QVBoxLayout>

#include <optional>

namespace l10n = localization;

namespace {
constexpr int DepotIdColumnWidth = 170;
constexpr int DepotRowHeight = 44;
constexpr int VisibleDepotRows = 3;
constexpr int UploadProgressStart = 70;
constexpr int UploadProgressComplete = 99;

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

QPushButton *createIconButton(const QString &text)
{
    auto *button = createActionButton(text, "iconButton");
    button->setFixedSize(36, 32);
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

int uploadProgressValue(double steamPercent)
{
    const auto boundedPercent = qBound(0.0, steamPercent, 100.0);
    const auto uploadSpan = UploadProgressComplete - UploadProgressStart;
    return qBound(UploadProgressStart,
                  UploadProgressStart + qRound((boundedPercent / 100.0) * uploadSpan),
                  UploadProgressComplete);
}

std::optional<double> latestSteamProgressPercent(const QString &output)
{
    static const QRegularExpression percentPattern(QStringLiteral(R"((\d{1,3}(?:[.,]\d+)?)\s*%)"));
    static const QRegularExpression progressPattern(
        QStringLiteral(R"(\bprogress:\s*(\d{1,3}(?:[.,]\d+)?))"),
        QRegularExpression::CaseInsensitiveOption);

    std::optional<double> latestProgress;
    qsizetype latestPosition = -1;
    const auto collectProgress = [&](const QRegularExpression &pattern) {
        auto matches = pattern.globalMatch(output);
        while (matches.hasNext()) {
            const auto match = matches.next();
            bool ok = false;
            auto valueText = match.captured(1);
            valueText.replace(',', '.');
            const auto value = valueText.toDouble(&ok);
            if (!ok || value < 0.0 || value > 100.0) {
                continue;
            }

            const auto position = match.capturedStart(1);
            if (position > latestPosition) {
                latestPosition = position;
                latestProgress = value;
            }
        }
    };

    collectProgress(percentPattern);
    collectProgress(progressPattern);
    return latestProgress;
}

QString tableText(const QTableWidget *table, int row, int column)
{
    const auto *item = table->item(row, column);
    return item ? item->text().trimmed() : QString{};
}

void setTableText(QTableWidget *table, int row, int column, const QString &text)
{
    auto *item = table->item(row, column);
    if (!item) {
        item = new QTableWidgetItem;
        table->setItem(row, column, item);
    }

    item->setText(text);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

QString vdfValue(QString value)
{
    value = QDir::fromNativeSeparators(value);
    value.replace('"', "\\\"");
    return value;
}

QString quotedVdfValue(const QString &value)
{
    return QStringLiteral("\"%1\"").arg(vdfValue(value));
}

bool writeTextFile(const QString &filePath, const QString &content, QString *errorMessage)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    if (file.write(content.toUtf8()) == -1) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    return true;
}

QStringList portableRootCandidates()
{
    QStringList roots;
    const QStringList basePaths = {
        QCoreApplication::applicationDirPath(),
        QDir::currentPath(),
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

bool isFileSystemRoot(const QString &path)
{
    const QDir directory(path);
    const auto absolutePath = QDir::cleanPath(directory.absolutePath());
    const auto rootPath = QDir::cleanPath(directory.rootPath());
#ifdef Q_OS_WIN
    return absolutePath.compare(rootPath, Qt::CaseInsensitive) == 0;
#else
    return absolutePath == rootPath;
#endif
}

bool canStoreAsPortableRelativePath(const QString &relativePath)
{
    const auto normalizedPath = QDir::fromNativeSeparators(relativePath);
    return !normalizedPath.isEmpty()
        && normalizedPath != QStringLiteral(".")
        && normalizedPath != QStringLiteral("..")
        && !normalizedPath.startsWith(QStringLiteral("../"))
        && !QFileInfo(normalizedPath).isAbsolute();
}

QString portableFallbackRoot(const QStringList &roots)
{
    for (const auto &root : roots) {
        if (!isFileSystemRoot(root)) {
            return root;
        }
    }

    return roots.isEmpty() ? QCoreApplication::applicationDirPath() : roots.first();
}

QStringList volumeRootCandidates()
{
    QStringList roots;
    const QStringList basePaths = {
        QCoreApplication::applicationDirPath(),
        QDir::currentPath(),
        QDir::homePath(),
    };

    for (const auto &basePath : basePaths) {
        const auto rootPath = QDir::cleanPath(QDir(basePath).rootPath());
        if (!roots.contains(rootPath, Qt::CaseInsensitive)) {
            roots.append(rootPath);
        }
    }

    return roots;
}

QString existingVolumeRootPath(QString relativePath)
{
    relativePath = QDir::fromNativeSeparators(relativePath.trimmed());
    while (relativePath.startsWith(QStringLiteral("/"))) {
        relativePath.remove(0, 1);
    }
    if (relativePath.isEmpty() || QFileInfo(relativePath).isAbsolute()) {
        return {};
    }

    for (const auto &root : volumeRootCandidates()) {
        const auto candidatePath = QDir(root).filePath(relativePath);
        if (QFileInfo::exists(candidatePath)) {
            return QDir::toNativeSeparators(QFileInfo(candidatePath).absoluteFilePath());
        }
    }

    return {};
}

QString steamCmdPath(const QString &contentBuilderPath)
{
    const auto contentBuilderSteamCmd = QDir(contentBuilderPath).filePath(QStringLiteral("builder/steamcmd.exe"));
    if (QFileInfo::exists(contentBuilderSteamCmd)) {
        return contentBuilderSteamCmd;
    }

    const auto siblingSteamCmd = QFileInfo(contentBuilderPath).absoluteDir().filePath(QStringLiteral("steamcmd.exe"));
    if (QFileInfo::exists(siblingSteamCmd)) {
        return siblingSteamCmd;
    }

    return contentBuilderSteamCmd;
}

bool samePath(const QString &left, const QString &right)
{
    const auto cleanLeft = QDir::cleanPath(QFileInfo(left).absoluteFilePath());
    const auto cleanRight = QDir::cleanPath(QFileInfo(right).absoluteFilePath());
#ifdef Q_OS_WIN
    return cleanLeft.compare(cleanRight, Qt::CaseInsensitive) == 0;
#else
    return cleanLeft == cleanRight;
#endif
}

bool isContentBuilderFolder(const QString &path)
{
    return QFileInfo::exists(path)
        && QFileInfo(path).isDir()
        && QFileInfo::exists(steamCmdPath(path));
}

QString contentBuilderFromSteamworksSdk(const QString &path)
{
    const auto resolvedPath = QDir::toNativeSeparators(QFileInfo(path).absoluteFilePath());
    const QStringList candidates = {
        resolvedPath,
        QDir(resolvedPath).filePath(QStringLiteral("tools/ContentBuilder")),
        QDir(resolvedPath).filePath(QStringLiteral("sdk/tools/ContentBuilder")),
    };

    for (const auto &candidate : candidates) {
        if (isContentBuilderFolder(candidate)) {
            return QDir::toNativeSeparators(QFileInfo(candidate).absoluteFilePath());
        }
    }

    return {};
}

bool removePath(const QString &path, QString *errorMessage)
{
    const QFileInfo pathInfo(path);
    if (!pathInfo.exists()) {
        return true;
    }

    if (pathInfo.isDir()) {
        QDir directory(path);
        if (directory.removeRecursively()) {
            return true;
        }
    } else if (QFile::remove(path)) {
        return true;
    }

    if (errorMessage) {
        *errorMessage = QStringLiteral("Could not remove %1").arg(QDir::toNativeSeparators(path));
    }
    return false;
}

bool copyDirectoryTree(const QString &sourcePath, const QString &targetPath, QString *errorMessage)
{
    QDir sourceDirectory(sourcePath);
    if (!sourceDirectory.exists()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Source folder does not exist: %1").arg(QDir::toNativeSeparators(sourcePath));
        }
        return false;
    }

    if (!QDir().mkpath(targetPath)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Could not create folder: %1").arg(QDir::toNativeSeparators(targetPath));
        }
        return false;
    }

    const auto entries = sourceDirectory.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    for (const auto &entry : entries) {
        const auto targetEntryPath = QDir(targetPath).filePath(entry.fileName());
        if (entry.isDir()) {
            if (!copyDirectoryTree(entry.absoluteFilePath(), targetEntryPath, errorMessage)) {
                return false;
            }
            continue;
        }

        if (QFileInfo::exists(targetEntryPath) && !QFile::remove(targetEntryPath)) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("Could not overwrite %1").arg(QDir::toNativeSeparators(targetEntryPath));
            }
            return false;
        }
        if (!QFile::copy(entry.absoluteFilePath(), targetEntryPath)) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("Could not copy %1 to %2")
                                    .arg(QDir::toNativeSeparators(entry.absoluteFilePath()),
                                         QDir::toNativeSeparators(targetEntryPath));
            }
            return false;
        }
    }

    return true;
}

bool updateContentBuilderFiles(const QString &sourcePath, const QString &targetPath, QString *errorMessage)
{
    if (samePath(sourcePath, targetPath)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Source and target ContentBuilder folders are the same.");
        }
        return false;
    }

    QDir sourceDirectory(sourcePath);
    if (!sourceDirectory.exists()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Source ContentBuilder folder does not exist.");
        }
        return false;
    }
    if (!QDir().mkpath(targetPath)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Could not create target ContentBuilder folder.");
        }
        return false;
    }

    const QStringList preservedDirectories = {
        QStringLiteral("content"),
        QStringLiteral("output"),
        QStringLiteral("scripts"),
    };
    const auto entries = sourceDirectory.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    for (const auto &entry : entries) {
        const auto targetEntryPath = QDir(targetPath).filePath(entry.fileName());
        if (entry.isDir() && preservedDirectories.contains(entry.fileName(), Qt::CaseInsensitive) && QFileInfo::exists(targetEntryPath)) {
            continue;
        }

        if (!removePath(targetEntryPath, errorMessage)) {
            return false;
        }
        if (entry.isDir()) {
            if (!copyDirectoryTree(entry.absoluteFilePath(), targetEntryPath, errorMessage)) {
                return false;
            }
            continue;
        }

        if (!QFile::copy(entry.absoluteFilePath(), targetEntryPath)) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("Could not copy %1").arg(QDir::toNativeSeparators(entry.absoluteFilePath()));
            }
            return false;
        }
    }

    return true;
}

QString defaultContentBuilderPath()
{
    for (const auto &root : portableRootCandidates()) {
        const auto candidatePath = QDir(root).filePath(QStringLiteral("Steam/ContentBuilder"));
        if (QFileInfo::exists(candidatePath) && QFileInfo::exists(steamCmdPath(candidatePath))) {
            return QDir::toNativeSeparators(QFileInfo(candidatePath).absoluteFilePath());
        }
    }

    return QDir::toNativeSeparators(QDir(QDir::currentPath()).filePath(QStringLiteral("Steam/ContentBuilder")));
}

QString resolvePortablePath(const QString &path)
{
    const auto trimmedPath = path.trimmed();
    if (trimmedPath.isEmpty()) {
        return {};
    }

    const auto normalizedPath = QDir::fromNativeSeparators(trimmedPath);
    const auto roots = portableRootCandidates();
    if (QFileInfo(normalizedPath).isAbsolute()) {
        if (!QFileInfo::exists(normalizedPath)
            && normalizedPath.contains(QStringLiteral("/Steam/ContentBuilder"), Qt::CaseInsensitive)) {
            return defaultContentBuilderPath();
        }

        return QDir::toNativeSeparators(QFileInfo(normalizedPath).absoluteFilePath());
    }

    for (const auto &root : roots) {
        const auto candidatePath = QDir(root).filePath(normalizedPath);
        if (QFileInfo::exists(candidatePath)) {
            return QDir::toNativeSeparators(QFileInfo(candidatePath).absoluteFilePath());
        }
    }

    const auto volumeRootPath = existingVolumeRootPath(normalizedPath);
    if (!volumeRootPath.isEmpty()) {
        return volumeRootPath;
    }

    return QDir::toNativeSeparators(QFileInfo(QDir(portableFallbackRoot(roots)).filePath(normalizedPath)).absoluteFilePath());
}

QString makePortablePath(const QString &path)
{
    const auto absolutePath = QFileInfo(resolvePortablePath(path)).absoluteFilePath();
    for (const auto &root : portableRootCandidates()) {
        if (isFileSystemRoot(root)) {
            continue;
        }

        const auto relativePath = QDir(root).relativeFilePath(absolutePath);
        if (canStoreAsPortableRelativePath(relativePath)) {
            return QDir::toNativeSeparators(relativePath);
        }
    }

    return QDir::toNativeSeparators(absolutePath);
}

QString scriptsPath(const QString &contentBuilderPath)
{
    return QDir(contentBuilderPath).filePath(QStringLiteral("scripts"));
}

QString outputPath(const QString &contentBuilderPath)
{
    return QDir(contentBuilderPath).filePath(QStringLiteral("output"));
}

QString appBuildFileName(const QString &appId)
{
    return QStringLiteral("app_build_%1.vdf").arg(appId);
}

QString depotBuildFileName(const QString &depotId)
{
    return QStringLiteral("depot_build_%1.vdf").arg(depotId);
}
}

SteamPage::SteamPage(ProjectRepository *repository, QWidget *parent)
    : QWidget(parent),
      m_repository(repository)
{
    setObjectName("page");
    setMinimumHeight(680);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(34, 28, 34, 32);
    layout->setSpacing(24);

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

    auto *appPanel = new QWidget;
    appPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    auto *appPanelLayout = new QVBoxLayout(appPanel);
    appPanelLayout->setContentsMargins(0, 0, 0, 0);
    appPanelLayout->setSpacing(12);

    auto *topGrid = new QGridLayout;
    topGrid->setContentsMargins(0, 0, 0, 0);
    topGrid->setHorizontalSpacing(14);
    topGrid->setVerticalSpacing(7);
    topGrid->setColumnStretch(0, 0);
    topGrid->setColumnStretch(1, 1);
    m_appIdLabel = createFieldLabel();
    m_buildDescriptionLabel = createFieldLabel();
    m_appIdEdit = createInput("480");
    m_buildDescriptionEdit = createInput("DevBuild");
    m_appIdEdit->setFixedWidth(180);
    topGrid->addWidget(m_appIdLabel, 0, 0);
    topGrid->addWidget(m_buildDescriptionLabel, 0, 1);
    topGrid->addWidget(m_appIdEdit, 1, 0);
    topGrid->addWidget(m_buildDescriptionEdit, 1, 1);
    appPanelLayout->addLayout(topGrid);
    layout->addWidget(appPanel);

    auto *depotPanel = new QWidget;
    depotPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto *depotPanelLayout = new QVBoxLayout(depotPanel);
    depotPanelLayout->setContentsMargins(0, 0, 0, 0);
    depotPanelLayout->setSpacing(14);

    auto *depotHeader = new QHBoxLayout;
    depotHeader->setContentsMargins(0, 0, 0, 0);
    depotHeader->setSpacing(8);
    m_depotConfigurationLabel = createSectionTitle();
    m_addDepotButton = createIconButton("+");
    m_removeDepotButton = createIconButton("-");
    m_browseDepotButton = createIconButton("...");
    depotHeader->addWidget(m_depotConfigurationLabel, 1);
    depotHeader->addWidget(m_addDepotButton);
    depotHeader->addWidget(m_removeDepotButton);
    depotHeader->addWidget(m_browseDepotButton);
    depotPanelLayout->addLayout(depotHeader);

    m_depotTable = new QTableWidget(0, 2);
    m_depotTable->setObjectName("depotTable");
    m_depotTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_depotTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_depotTable->horizontalHeader()->setMinimumHeight(38);
    m_depotTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    m_depotTable->horizontalHeader()->resizeSection(0, DepotIdColumnWidth);
    m_depotTable->horizontalHeader()->setStretchLastSection(true);
    m_depotTable->verticalHeader()->setVisible(false);
    m_depotTable->verticalHeader()->setDefaultSectionSize(DepotRowHeight);
    m_depotTable->verticalHeader()->setMinimumSectionSize(DepotRowHeight);
    m_depotTable->setAlternatingRowColors(true);
    m_depotTable->setMinimumHeight(m_depotTable->horizontalHeader()->minimumHeight()
                                   + DepotRowHeight * VisibleDepotRows
                                   + 12);
    m_depotTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    depotPanelLayout->addWidget(m_depotTable, 1);
    layout->addWidget(depotPanel, 5);

    auto *actionPanel = new QWidget;
    actionPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    auto *actionPanelLayout = new QVBoxLayout(actionPanel);
    actionPanelLayout->setContentsMargins(0, 0, 0, 0);
    actionPanelLayout->setSpacing(14);

    auto *buildActions = new QGridLayout;
    buildActions->setContentsMargins(0, 0, 0, 0);
    buildActions->setHorizontalSpacing(14);
    buildActions->setVerticalSpacing(8);
    buildActions->setColumnStretch(2, 1);
    m_branchLabel = createFieldLabel();
    m_branchEdit = createInput("beta");
    m_branchEdit->setFixedWidth(180);
    m_previewBuildCheckBox = new QCheckBox;
    m_validateButton = createActionButton({});
    m_generateButton = createActionButton({});
    m_uploadButton = createActionButton({}, "primaryButton");
    m_validateButton->setMinimumHeight(42);
    m_generateButton->setMinimumHeight(42);
    m_uploadButton->setMinimumHeight(42);
    buildActions->addWidget(m_branchLabel, 0, 0);
    buildActions->addWidget(m_branchEdit, 1, 0);
    buildActions->addWidget(m_previewBuildCheckBox, 1, 1);
    buildActions->addWidget(m_validateButton, 1, 3);
    buildActions->addWidget(m_generateButton, 1, 4);
    buildActions->addWidget(m_uploadButton, 1, 5);
    actionPanelLayout->addLayout(buildActions);

    m_progressBar = new QProgressBar;
    m_progressBar->setObjectName("steamProgressBar");
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setMinimumHeight(32);
    actionPanelLayout->addWidget(m_progressBar);
    layout->addWidget(actionPanel);

    auto *configPanel = new QWidget;
    configPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    auto *configLayout = new QVBoxLayout(configPanel);
    configLayout->setContentsMargins(0, 0, 0, 0);
    configLayout->setSpacing(14);

    m_steamConfigurationLabel = createSectionTitle();
    configLayout->addWidget(m_steamConfigurationLabel);

    auto *configGrid = new QGridLayout;
    configGrid->setContentsMargins(0, 0, 0, 0);
    configGrid->setHorizontalSpacing(14);
    configGrid->setVerticalSpacing(7);
    configGrid->setColumnStretch(0, 2);
    configGrid->setColumnStretch(1, 1);
    configGrid->setColumnStretch(2, 1);
    m_contentBuilderPathLabel = createFieldLabel();
    m_loginLabel = createFieldLabel();
    m_passwordLabel = createFieldLabel();

    auto *contentBuilderRow = new QHBoxLayout;
    contentBuilderRow->setSpacing(10);
    m_contentBuilderPathEdit = createInput(defaultContentBuilderPath());
    m_browseContentBuilderButton = createActionButton({});
    contentBuilderRow->addWidget(m_contentBuilderPathEdit, 1);
    contentBuilderRow->addWidget(m_browseContentBuilderButton);

    m_loginEdit = createInput("steam_account");
    m_passwordEdit = createInput({});
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_showPasswordCheckBox = new QCheckBox;
    m_savePasswordCheckBox = new QCheckBox;

    auto *passwordOptions = new QHBoxLayout;
    passwordOptions->setContentsMargins(0, 0, 0, 0);
    passwordOptions->setSpacing(12);
    passwordOptions->addWidget(m_showPasswordCheckBox);
    passwordOptions->addWidget(m_savePasswordCheckBox);
    passwordOptions->addStretch(1);

    configGrid->addWidget(m_contentBuilderPathLabel, 0, 0);
    configGrid->addWidget(m_loginLabel, 0, 1);
    configGrid->addWidget(m_passwordLabel, 0, 2);
    configGrid->addLayout(contentBuilderRow, 1, 0);
    configGrid->addWidget(m_loginEdit, 1, 1);
    configGrid->addWidget(m_passwordEdit, 1, 2);
    configGrid->addLayout(passwordOptions, 2, 2);
    configLayout->addLayout(configGrid);

    auto *configButtons = new QHBoxLayout;
    configButtons->addStretch(1);
    m_updateSdkButton = createActionButton({});
    m_viewLogsFolderButton = createActionButton({});
    configButtons->addWidget(m_updateSdkButton);
    configButtons->addWidget(m_viewLogsFolderButton);
    configLayout->addLayout(configButtons);
    layout->addWidget(configPanel);

    m_uploadProcess = new QProcess(this);

    connect(m_projectComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SteamPage::loadProject);
    connect(m_saveProjectButton, &QPushButton::clicked, this, &SteamPage::saveCurrentProject);
    connect(m_addDepotButton, &QPushButton::clicked, this, &SteamPage::addDepot);
    connect(m_removeDepotButton, &QPushButton::clicked, this, &SteamPage::removeDepot);
    connect(m_browseDepotButton, &QPushButton::clicked, this, &SteamPage::browseSelectedDepotPath);
    connect(m_browseContentBuilderButton, &QPushButton::clicked, this, &SteamPage::browseContentBuilderPath);
    connect(m_validateButton, &QPushButton::clicked, this, [this] {
        validateConfiguration();
    });
    connect(m_generateButton, &QPushButton::clicked, this, [this] {
        generateVdfs();
    });
    connect(m_uploadButton, &QPushButton::clicked, this, &SteamPage::uploadToSteam);
    connect(m_updateSdkButton, &QPushButton::clicked, this, &SteamPage::updateContentBuilderFromSdk);
    connect(m_viewLogsFolderButton, &QPushButton::clicked, this, &SteamPage::openLogsFolder);
    connect(m_showPasswordCheckBox, &QCheckBox::toggled, this, &SteamPage::updatePasswordEchoMode);
    connect(m_uploadProcess, &QProcess::readyReadStandardOutput, this, &SteamPage::appendProcessOutput);
    connect(m_uploadProcess, &QProcess::readyReadStandardError, this, &SteamPage::appendProcessOutput);
    connect(m_uploadProcess, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        appendUploadLog(QStringLiteral("steamcmd finished with exit code %1.").arg(exitCode));
        if (exitCode == 0) {
            setProgress(100, l10n::translate(m_language, l10n::Text::SteamProgressComplete), QStringLiteral("complete"));
            emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamUploadFinished));
        } else {
            setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
            emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamUploadFailed)
                                  .arg(QStringLiteral("exit code %1").arg(exitCode)));
        }
    });
    connect(m_uploadProcess, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
        const auto message = m_uploadProcess->errorString();
        appendUploadLog(message);
        setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamUploadFailed).arg(message));
    });

    if (m_repository) {
        connect(m_repository, &ProjectRepository::projectsChanged, this, [this] {
            refreshProjects(currentProjectRow());
        });
    }

    updateTexts();
    setProgress(0, l10n::translate(m_language, l10n::Text::SteamProgressReady));
    refreshProjects();
}

void SteamPage::refreshProjects(int preferredRow)
{
    if (!m_repository) {
        clearSteamForm();
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
            m_projectComboBox->addItem(QStringLiteral("New Steam Project"), -1);
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

void SteamPage::loadProject(int comboIndex)
{
    Q_UNUSED(comboIndex)

    const auto row = currentProjectRow();
    if (!m_repository || row < 0 || row >= m_repository->projects().size()) {
        prepareDraftProject();
        return;
    }

    const auto &project = m_repository->projects().at(row);
    setSteamFormEnabled(true);
    m_appIdEdit->setText(project.steamAppId());
    m_buildDescriptionEdit->setText(project.steamBuildDescription().isEmpty()
                                        ? QStringLiteral("DevBuild")
                                        : project.steamBuildDescription());
    m_branchEdit->setText(project.steamBranch());
    m_contentBuilderPathEdit->setText(project.steamContentBuilderPath().isEmpty()
                                          ? defaultContentBuilderPath()
                                          : resolvePortablePath(project.steamContentBuilderPath()));
    m_loginEdit->setText(project.steamLogin());
    m_savePasswordCheckBox->setChecked(project.steamSavePassword());
    m_passwordEdit->setText(project.steamSavePassword() ? project.steamPassword() : QString{});
    m_previewBuildCheckBox->setChecked(project.steamPreviewBuild());

    m_depotTable->setRowCount(0);
    auto depots = project.steamDepots();
    if (depots.isEmpty() && !project.buildDirectory().isEmpty()) {
        depots.append(SteamDepotConfig{{}, project.buildDirectory()});
    }
    for (const auto &depot : depots) {
        const auto rowIndex = m_depotTable->rowCount();
        m_depotTable->insertRow(rowIndex);
        m_depotTable->setRowHeight(rowIndex, DepotRowHeight);
        setTableText(m_depotTable, rowIndex, 0, depot.depotId);
        setTableText(m_depotTable, rowIndex, 1, resolvePortablePath(depot.buildPath));
    }

    updatePasswordEchoMode();
}

void SteamPage::prepareDraftProject()
{
    setSteamFormEnabled(true);
    m_appIdEdit->clear();
    m_buildDescriptionEdit->setText(QStringLiteral("DevBuild"));
    m_branchEdit->clear();
    m_contentBuilderPathEdit->setText(defaultContentBuilderPath());
    m_loginEdit->clear();
    m_passwordEdit->clear();
    m_previewBuildCheckBox->setChecked(false);
    m_showPasswordCheckBox->setChecked(false);
    m_savePasswordCheckBox->setChecked(false);
    m_depotTable->setRowCount(0);
    m_depotTable->insertRow(0);
    m_depotTable->setRowHeight(0, DepotRowHeight);
    setTableText(m_depotTable, 0, 0, {});
    setTableText(m_depotTable, 0, 1, {});
    m_depotTable->clearSelection();
    setProgress(0, l10n::translate(m_language, l10n::Text::SteamProgressReady));
}

void SteamPage::clearSteamForm()
{
    m_appIdEdit->clear();
    m_buildDescriptionEdit->clear();
    m_branchEdit->clear();
    m_contentBuilderPathEdit->setText(defaultContentBuilderPath());
    m_loginEdit->clear();
    m_passwordEdit->clear();
    m_depotTable->setRowCount(0);
    m_previewBuildCheckBox->setChecked(false);
    m_showPasswordCheckBox->setChecked(false);
    m_savePasswordCheckBox->setChecked(false);
    setProgress(0, l10n::translate(m_language, l10n::Text::SteamProgressReady));
    setSteamFormEnabled(false);
}

void SteamPage::setSteamFormEnabled(bool enabled)
{
    m_saveProjectButton->setEnabled(enabled);
    m_appIdEdit->setEnabled(enabled);
    m_buildDescriptionEdit->setEnabled(enabled);
    m_branchEdit->setEnabled(enabled);
    m_contentBuilderPathEdit->setEnabled(enabled);
    m_loginEdit->setEnabled(enabled);
    m_passwordEdit->setEnabled(enabled);
    m_depotTable->setEnabled(enabled);
    m_previewBuildCheckBox->setEnabled(enabled);
    m_showPasswordCheckBox->setEnabled(enabled);
    m_savePasswordCheckBox->setEnabled(enabled);
    m_validateButton->setEnabled(enabled);
    m_addDepotButton->setEnabled(enabled);
    m_removeDepotButton->setEnabled(enabled);
    m_browseDepotButton->setEnabled(enabled);
    m_browseContentBuilderButton->setEnabled(enabled);
    m_generateButton->setEnabled(enabled);
    m_uploadButton->setEnabled(enabled);
    m_updateSdkButton->setEnabled(enabled);
    m_viewLogsFolderButton->setEnabled(enabled);
}

void SteamPage::saveCurrentProject()
{
    setProgress(30, l10n::translate(m_language, l10n::Text::SteamProgressSaving));
    if (persistCurrentProject(true)) {
        setProgress(35, l10n::translate(m_language, l10n::Text::SteamProgressSaved));
    }
}

bool SteamPage::persistCurrentProject(bool writeLog)
{
    if (!m_repository) {
        return false;
    }

    const auto row = currentProjectRow();
    const bool updateExistingProject = row >= 0 && row < m_repository->projects().size();
    const auto depots = collectDepots(true);
    const auto resolvedDepots = collectDepots(false);
    const auto appId = m_appIdEdit->text().trimmed();

    auto project = updateExistingProject
                       ? m_repository->projects().at(row)
                       : GameProject(appId.isEmpty() ? QStringLiteral("New Steam Game")
                                                     : QStringLiteral("Steam App %1").arg(appId),
                                     {},
                                     resolvedDepots.isEmpty() ? QString{} : resolvedDepots.first().buildPath,
                                     QStringLiteral("0.1.0"),
                                     {},
                                     {QStringLiteral("Steam")},
                                     QDateTime::currentDateTime());
    project.setSteamAppId(m_appIdEdit->text().trimmed());
    project.setSteamBuildDescription(m_buildDescriptionEdit->text().trimmed());
    project.setSteamDepots(depots);
    const auto contentBuilderPath = m_contentBuilderPathEdit->text().trimmed().isEmpty()
                                        ? defaultContentBuilderPath()
                                        : resolvePortablePath(m_contentBuilderPathEdit->text().trimmed());
    project.setSteamContentBuilderPath(makePortablePath(contentBuilderPath));
    project.setSteamBranch(m_branchEdit->text().trimmed());
    project.setSteamLogin(m_loginEdit->text().trimmed());
    project.setSteamSavePassword(m_savePasswordCheckBox->isChecked());
    project.setSteamPassword(m_savePasswordCheckBox->isChecked() ? m_passwordEdit->text() : QString{});
    project.setSteamPreviewBuild(m_previewBuildCheckBox->isChecked());
    project.setLastUpdated(QDateTime::currentDateTime());

    auto targets = project.uploadTargets();
    if (!targets.contains(QStringLiteral("Steam"), Qt::CaseInsensitive)) {
        targets.append(QStringLiteral("Steam"));
        project.setUploadTargets(targets);
    }

    const auto savedRow = updateExistingProject ? row : m_repository->addProject(project);
    if (updateExistingProject) {
        m_repository->updateProject(row, project);
    }
    refreshProjects(savedRow);
    if (writeLog) {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamProjectSaved).arg(project.name()));
    }
    return true;
}

void SteamPage::addDepot()
{
    const auto row = m_depotTable->rowCount();
    m_depotTable->insertRow(row);
    m_depotTable->setRowHeight(row, DepotRowHeight);
    setTableText(m_depotTable, row, 0, {});
    setTableText(m_depotTable, row, 1, {});
    m_depotTable->setCurrentCell(row, 0);
    emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamDepotAdded));
}

void SteamPage::removeDepot()
{
    auto row = m_depotTable->currentRow();
    if (row < 0) {
        row = m_depotTable->rowCount() - 1;
    }

    if (row < 0) {
        return;
    }

    m_depotTable->removeRow(row);
    emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamDepotRemoved));
}

void SteamPage::browseSelectedDepotPath()
{
    auto row = m_depotTable->currentRow();
    if (row < 0 && m_depotTable->rowCount() > 0) {
        row = 0;
    }
    if (row < 0) {
        addDepot();
        row = 0;
    }

    const auto folder = QFileDialog::getExistingDirectory(this,
                                                          l10n::translate(m_language, l10n::Text::BuildPath),
                                                          tableText(m_depotTable, row, 1));
    if (folder.isEmpty()) {
        return;
    }

    setTableText(m_depotTable, row, 1, QDir::toNativeSeparators(folder));
}

void SteamPage::browseContentBuilderPath()
{
    const auto folder = QFileDialog::getExistingDirectory(this,
                                                          l10n::translate(m_language, l10n::Text::ContentBuilderPath),
                                                          m_contentBuilderPathEdit->text().trimmed().isEmpty()
                                                              ? defaultContentBuilderPath()
                                                              : m_contentBuilderPathEdit->text());
    if (folder.isEmpty()) {
        return;
    }

    m_contentBuilderPathEdit->setText(QDir::toNativeSeparators(folder));
}

void SteamPage::updateContentBuilderFromSdk()
{
    const auto currentContentBuilderPath = m_contentBuilderPathEdit->text().trimmed().isEmpty()
                                               ? defaultContentBuilderPath()
                                               : resolvePortablePath(m_contentBuilderPathEdit->text().trimmed());
    const auto selectedPath = QFileDialog::getExistingDirectory(this,
                                                                l10n::translate(m_language, l10n::Text::SteamworksSdkPath),
                                                                QFileInfo(currentContentBuilderPath).absolutePath());
    if (selectedPath.isEmpty()) {
        return;
    }

    const auto sourceContentBuilderPath = contentBuilderFromSteamworksSdk(selectedPath);
    if (sourceContentBuilderPath.isEmpty()) {
        const auto message = QStringLiteral("Selected folder does not contain sdk/tools/ContentBuilder with steamcmd.exe.");
        appendUploadLog(message);
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamSdkUpdateFailed).arg(message));
        return;
    }

    QString errorMessage;
    if (!updateContentBuilderFiles(sourceContentBuilderPath, currentContentBuilderPath, &errorMessage)) {
        appendUploadLog(errorMessage);
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamSdkUpdateFailed).arg(errorMessage));
        return;
    }

    m_contentBuilderPathEdit->setText(QDir::toNativeSeparators(currentContentBuilderPath));
    appendUploadLog(QStringLiteral("Updated ContentBuilder from %1").arg(QDir::toNativeSeparators(sourceContentBuilderPath)));
    emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamSdkUpdated)
                          .arg(QDir::toNativeSeparators(sourceContentBuilderPath)));
}

bool SteamPage::validateConfiguration()
{
    setProgress(10, l10n::translate(m_language, l10n::Text::SteamProgressValidating));

    QStringList missing;
    if (m_appIdEdit->text().trimmed().isEmpty()) {
        missing.append(l10n::translate(m_language, l10n::Text::AppId));
    }
    if (m_contentBuilderPathEdit->text().trimmed().isEmpty()) {
        missing.append(l10n::translate(m_language, l10n::Text::ContentBuilderPath));
    }

    const auto depots = collectDepots(false);
    if (depots.isEmpty()) {
        missing.append(l10n::translate(m_language, l10n::Text::DepotConfiguration));
    }
    for (const auto &depot : depots) {
        if (depot.depotId.isEmpty()) {
            missing.append(l10n::translate(m_language, l10n::Text::DepotId));
            break;
        }
    }
    for (const auto &depot : depots) {
        if (depot.buildPath.isEmpty()) {
            missing.append(l10n::translate(m_language, l10n::Text::BuildPath));
            break;
        }
    }

    if (!missing.isEmpty()) {
        appendUploadLog(QStringLiteral("Missing configuration: %1").arg(missing.join(QStringLiteral(", "))));
        setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
        return false;
    }

    for (const auto &depot : depots) {
        if (!QFileInfo(depot.buildPath).isDir()) {
            appendUploadLog(QStringLiteral("Build path does not exist: %1").arg(QDir::toNativeSeparators(depot.buildPath)));
            setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
            return false;
        }
    }

    appendUploadLog(QStringLiteral("Steam configuration is complete."));
    setProgress(25, l10n::translate(m_language, l10n::Text::SteamProgressValidated));
    emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamValidate));
    return true;
}

bool SteamPage::generateVdfs()
{
    if (!validateConfiguration()) {
        return false;
    }

    setProgress(30, l10n::translate(m_language, l10n::Text::SteamProgressSaving));
    if (!persistCurrentProject(false)) {
        return false;
    }
    setProgress(45, l10n::translate(m_language, l10n::Text::SteamProgressGenerating));

    const auto contentBuilderPath = resolvePortablePath(m_contentBuilderPathEdit->text().trimmed());
    m_contentBuilderPathEdit->setText(contentBuilderPath);
    const auto scriptsFolder = scriptsPath(contentBuilderPath);
    const auto outputFolder = outputPath(contentBuilderPath);
    const auto contentFolder = QDir(contentBuilderPath).filePath(QStringLiteral("content"));
    if (!QDir().mkpath(scriptsFolder) || !QDir().mkpath(outputFolder) || !QDir().mkpath(contentFolder)) {
        appendUploadLog(QStringLiteral("Could not create SteamPipe folders."));
        setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
        return false;
    }

    const auto appId = m_appIdEdit->text().trimmed();
    const auto depots = collectDepots(false);
    QString errorMessage;
    for (const auto &depot : depots) {
        QString depotVdf;
        QTextStream depotStream(&depotVdf);
        depotStream << "\"DepotBuildConfig\"\n";
        depotStream << "{\n";
        depotStream << "    \"DepotID\" " << quotedVdfValue(depot.depotId) << "\n";
        depotStream << "    \"ContentRoot\" " << quotedVdfValue(depot.buildPath) << "\n";
        depotStream << "    \"FileMapping\"\n";
        depotStream << "    {\n";
        depotStream << "        \"LocalPath\" \"*\"\n";
        depotStream << "        \"DepotPath\" \".\"\n";
        depotStream << "        \"recursive\" \"1\"\n";
        depotStream << "    }\n";
        depotStream << "}\n";

        const auto depotFilePath = QDir(scriptsFolder).filePath(depotBuildFileName(depot.depotId));
        if (!writeTextFile(depotFilePath, depotVdf, &errorMessage)) {
            appendUploadLog(QStringLiteral("Could not write %1: %2").arg(depotFilePath, errorMessage));
            setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
            return false;
        }
    }

    QString appVdf;
    QTextStream appStream(&appVdf);
    appStream << "\"AppBuild\"\n";
    appStream << "{\n";
    appStream << "    \"AppID\" " << quotedVdfValue(appId) << "\n";
    appStream << "    \"Desc\" " << quotedVdfValue(m_buildDescriptionEdit->text().trimmed()) << "\n";
    appStream << "    \"BuildOutput\" " << quotedVdfValue(outputFolder) << "\n";
    appStream << "    \"ContentRoot\" " << quotedVdfValue(contentFolder) << "\n";
    if (!m_branchEdit->text().trimmed().isEmpty()) {
        appStream << "    \"SetLive\" " << quotedVdfValue(m_branchEdit->text().trimmed()) << "\n";
    }
    if (m_previewBuildCheckBox->isChecked()) {
        appStream << "    \"Preview\" \"1\"\n";
    }
    appStream << "    \"Depots\"\n";
    appStream << "    {\n";
    for (const auto &depot : depots) {
        appStream << "        " << quotedVdfValue(depot.depotId) << " "
                  << quotedVdfValue(depotBuildFileName(depot.depotId)) << "\n";
    }
    appStream << "    }\n";
    appStream << "}\n";

    const auto appFilePath = QDir(scriptsFolder).filePath(appBuildFileName(appId));
    if (!writeTextFile(appFilePath, appVdf, &errorMessage)) {
        appendUploadLog(QStringLiteral("Could not write %1: %2").arg(appFilePath, errorMessage));
        setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
        return false;
    }

    appendUploadLog(QStringLiteral("Generated SteamPipe VDFs in %1").arg(QDir::toNativeSeparators(scriptsFolder)));
    setProgress(70, l10n::translate(m_language, l10n::Text::SteamProgressGenerated));
    emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamVdfGenerated)
                          .arg(QDir::toNativeSeparators(scriptsFolder)));
    return true;
}

void SteamPage::uploadToSteam()
{
    if (!generateVdfs()) {
        return;
    }

    const auto contentBuilderPath = resolvePortablePath(m_contentBuilderPathEdit->text().trimmed());
    m_contentBuilderPathEdit->setText(contentBuilderPath);
    const auto executablePath = steamCmdPath(contentBuilderPath);
    if (!QFileInfo::exists(executablePath)) {
        const auto message = QStringLiteral("steamcmd.exe not found at %1").arg(QDir::toNativeSeparators(executablePath));
        appendUploadLog(message);
        setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamUploadFailed).arg(message));
        return;
    }

    if (m_loginEdit->text().trimmed().isEmpty()) {
        const auto message = QStringLiteral("Steam login is empty.");
        appendUploadLog(message);
        setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamUploadFailed).arg(message));
        return;
    }

    if (m_uploadProcess->state() != QProcess::NotRunning) {
        appendUploadLog(QStringLiteral("steamcmd is already running."));
        return;
    }

    const auto appBuildPath = QDir(scriptsPath(contentBuilderPath)).filePath(appBuildFileName(m_appIdEdit->text().trimmed()));
    QStringList arguments;
    arguments << QStringLiteral("+login") << m_loginEdit->text().trimmed();
    if (!m_passwordEdit->text().isEmpty()) {
        arguments << m_passwordEdit->text();
    }
    arguments << QStringLiteral("+run_app_build") << appBuildPath << QStringLiteral("+quit");

    m_uploadProcess->setProgram(executablePath);
    m_uploadProcess->setArguments(arguments);
    m_uploadProcess->setWorkingDirectory(contentBuilderPath);
    m_uploadProcess->start();
    if (!m_uploadProcess->waitForStarted(1500)) {
        const auto message = m_uploadProcess->errorString();
        appendUploadLog(message);
        setProgressError(l10n::translate(m_language, l10n::Text::SteamProgressFailed));
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamUploadFailed).arg(message));
        return;
    }

    appendUploadLog(QStringLiteral("Started steamcmd upload."));
    setProgressBusy(l10n::translate(m_language, l10n::Text::SteamProgressUploading));
    emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamUploadStarted));
}

void SteamPage::openLogsFolder()
{
    const auto folder = outputPath(resolvePortablePath(m_contentBuilderPathEdit->text().trimmed()));
    QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
}

void SteamPage::appendUploadLog(const QString &message)
{
    emit logRequested(QStringLiteral("Steam: %1").arg(message));
}

void SteamPage::appendProcessOutput()
{
    const auto output = QString::fromLocal8Bit(m_uploadProcess->readAllStandardOutput())
                            + QString::fromLocal8Bit(m_uploadProcess->readAllStandardError());
    if (!output.trimmed().isEmpty()) {
        appendUploadLog(output.trimmed());
    }
    updateUploadProgressFromOutput(output);
}

void SteamPage::updateUploadProgressFromOutput(const QString &output)
{
    if (m_uploadProcess->state() == QProcess::NotRunning) {
        return;
    }

    const auto progress = latestSteamProgressPercent(output);
    if (!progress.has_value()) {
        return;
    }

    const auto nextValue = uploadProgressValue(*progress);
    setProgress(qMax(m_progressBar->value(), nextValue),
                l10n::translate(m_language, l10n::Text::SteamProgressUploading));
}

void SteamPage::setProgress(int value, const QString &text, const QString &state)
{
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(qBound(0, value, 100));
    m_progressBar->setFormat(QStringLiteral("%1 - %p%").arg(text));
    m_progressBar->setProperty("state", state.isEmpty() ? QStringLiteral("active") : state);
    m_progressBar->style()->unpolish(m_progressBar);
    m_progressBar->style()->polish(m_progressBar);
}

void SteamPage::setProgressBusy(const QString &text)
{
    m_progressBar->setRange(0, 0);
    m_progressBar->setFormat(text);
    m_progressBar->setProperty("state", QStringLiteral("active"));
    m_progressBar->style()->unpolish(m_progressBar);
    m_progressBar->style()->polish(m_progressBar);
}

void SteamPage::setProgressError(const QString &text)
{
    setProgress(0, text, QStringLiteral("error"));
}

void SteamPage::updatePasswordEchoMode()
{
    m_passwordEdit->setEchoMode(m_showPasswordCheckBox->isChecked() ? QLineEdit::Normal : QLineEdit::Password);
}

int SteamPage::currentProjectRow() const
{
    const auto data = m_projectComboBox->currentData();
    return data.isValid() ? data.toInt() : -1;
}

QVector<SteamDepotConfig> SteamPage::collectDepots(bool portable) const
{
    QVector<SteamDepotConfig> depots;
    depots.reserve(m_depotTable->rowCount());
    for (int row = 0; row < m_depotTable->rowCount(); ++row) {
        SteamDepotConfig depot;
        depot.depotId = tableText(m_depotTable, row, 0);
        const auto buildPath = tableText(m_depotTable, row, 1);
        depot.buildPath = buildPath.isEmpty() ? QString{} : (portable ? makePortablePath(buildPath) : resolvePortablePath(buildPath));
        if (!depot.depotId.isEmpty() || !depot.buildPath.isEmpty()) {
            depots.append(depot);
        }
    }

    return depots;
}

void SteamPage::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void SteamPage::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::SteamTitle));
    m_projectLabel->setText(l10n::translate(m_language, l10n::Text::Project));
    m_saveProjectButton->setText(l10n::translate(m_language, l10n::Text::SaveSettings));
    m_appIdLabel->setText(l10n::translate(m_language, l10n::Text::AppId));
    m_buildDescriptionLabel->setText(l10n::translate(m_language, l10n::Text::BuildDescription));
    m_depotConfigurationLabel->setText(l10n::translate(m_language, l10n::Text::DepotConfiguration));
    m_branchLabel->setText(l10n::translate(m_language, l10n::Text::Branch));
    m_previewBuildCheckBox->setText(l10n::translate(m_language, l10n::Text::PreviewBuild));
    m_steamConfigurationLabel->setText(l10n::translate(m_language, l10n::Text::SteamConfiguration));
    m_contentBuilderPathLabel->setText(l10n::translate(m_language, l10n::Text::ContentBuilderPath));
    m_loginLabel->setText(l10n::translate(m_language, l10n::Text::SteamLogin));
    m_passwordLabel->setText(l10n::translate(m_language, l10n::Text::SteamPassword));
    m_showPasswordCheckBox->setText(l10n::translate(m_language, l10n::Text::ShowPassword));
    m_savePasswordCheckBox->setText(l10n::translate(m_language, l10n::Text::SavePassword));
    m_validateButton->setText(l10n::translate(m_language, l10n::Text::ValidateConfig));
    m_addDepotButton->setText("+");
    m_addDepotButton->setToolTip(l10n::translate(m_language, l10n::Text::AddDepot));
    m_removeDepotButton->setText("-");
    m_removeDepotButton->setToolTip(l10n::translate(m_language, l10n::Text::RemoveDepot));
    m_browseDepotButton->setText("...");
    m_browseDepotButton->setToolTip(l10n::translate(m_language, l10n::Text::Browse));
    m_browseContentBuilderButton->setText(l10n::translate(m_language, l10n::Text::Browse));
    m_generateButton->setText(l10n::translate(m_language, l10n::Text::GenerateVdf));
    m_uploadButton->setText(l10n::translate(m_language, l10n::Text::UploadToSteam));
    m_updateSdkButton->setText(l10n::translate(m_language, l10n::Text::UpdateSteamSdk));
    m_viewLogsFolderButton->setText(l10n::translate(m_language, l10n::Text::ViewLogsFolder));
    m_depotTable->setHorizontalHeaderLabels({l10n::translate(m_language, l10n::Text::DepotId),
                                             l10n::translate(m_language, l10n::Text::BuildPath)});
}
