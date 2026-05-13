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
