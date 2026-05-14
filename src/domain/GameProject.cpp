#include "domain/GameProject.h"

#include <utility>

GameProject::GameProject(QString name,
                         QString projectRoot,
                         QString buildDirectory,
                         QString version,
                         QString changelog,
                         QStringList uploadTargets,
                         QDateTime lastUpdated)
    : m_name(std::move(name)),
      m_projectRoot(std::move(projectRoot)),
      m_buildDirectory(std::move(buildDirectory)),
      m_version(std::move(version)),
      m_changelog(std::move(changelog)),
      m_uploadTargets(std::move(uploadTargets)),
      m_lastUpdated(std::move(lastUpdated))
{
}

const QString &GameProject::name() const
{
    return m_name;
}

const QString &GameProject::projectRoot() const
{
    return m_projectRoot;
}

const QString &GameProject::buildDirectory() const
{
    return m_buildDirectory;
}

const QString &GameProject::version() const
{
    return m_version;
}

const QString &GameProject::changelog() const
{
    return m_changelog;
}

const QStringList &GameProject::uploadTargets() const
{
    return m_uploadTargets;
}

const QDateTime &GameProject::lastUpdated() const
{
    return m_lastUpdated;
}

const QString &GameProject::steamAppId() const
{
    return m_steamAppId;
}

const QString &GameProject::steamBuildDescription() const
{
    return m_steamBuildDescription;
}

const QVector<SteamDepotConfig> &GameProject::steamDepots() const
{
    return m_steamDepots;
}

const QString &GameProject::steamContentBuilderPath() const
{
    return m_steamContentBuilderPath;
}

const QString &GameProject::steamBranch() const
{
    return m_steamBranch;
}

const QString &GameProject::steamLogin() const
{
    return m_steamLogin;
}

const QString &GameProject::steamPassword() const
{
    return m_steamPassword;
}

bool GameProject::steamSavePassword() const
{
    return m_steamSavePassword;
}

bool GameProject::steamPreviewBuild() const
{
    return m_steamPreviewBuild;
}

void GameProject::setName(QString name)
{
    m_name = std::move(name);
}

void GameProject::setProjectRoot(QString projectRoot)
{
    m_projectRoot = std::move(projectRoot);
}

void GameProject::setBuildDirectory(QString buildDirectory)
{
    m_buildDirectory = std::move(buildDirectory);
}

void GameProject::setVersion(QString version)
{
    m_version = std::move(version);
}

void GameProject::setChangelog(QString changelog)
{
    m_changelog = std::move(changelog);
}

void GameProject::setUploadTargets(QStringList uploadTargets)
{
    m_uploadTargets = std::move(uploadTargets);
}

void GameProject::setLastUpdated(QDateTime lastUpdated)
{
    m_lastUpdated = std::move(lastUpdated);
}

void GameProject::setSteamAppId(QString steamAppId)
{
    m_steamAppId = std::move(steamAppId);
}

void GameProject::setSteamBuildDescription(QString steamBuildDescription)
{
    m_steamBuildDescription = std::move(steamBuildDescription);
}

void GameProject::setSteamDepots(QVector<SteamDepotConfig> steamDepots)
{
    m_steamDepots = std::move(steamDepots);
}

void GameProject::setSteamContentBuilderPath(QString steamContentBuilderPath)
{
    m_steamContentBuilderPath = std::move(steamContentBuilderPath);
}

void GameProject::setSteamBranch(QString steamBranch)
{
    m_steamBranch = std::move(steamBranch);
}

void GameProject::setSteamLogin(QString steamLogin)
{
    m_steamLogin = std::move(steamLogin);
}

void GameProject::setSteamPassword(QString steamPassword)
{
    m_steamPassword = std::move(steamPassword);
}

void GameProject::setSteamSavePassword(bool steamSavePassword)
{
    m_steamSavePassword = steamSavePassword;
}

void GameProject::setSteamPreviewBuild(bool steamPreviewBuild)
{
    m_steamPreviewBuild = steamPreviewBuild;
}
