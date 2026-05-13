#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>

class GameProject
{
public:
    GameProject(QString name,
                QString projectRoot,
                QString buildDirectory,
                QString version,
                QString changelog,
                QStringList uploadTargets,
                QDateTime lastUpdated);

    [[nodiscard]] const QString &name() const;
    [[nodiscard]] const QString &projectRoot() const;
    [[nodiscard]] const QString &buildDirectory() const;
    [[nodiscard]] const QString &version() const;
    [[nodiscard]] const QString &changelog() const;
    [[nodiscard]] const QStringList &uploadTargets() const;
    [[nodiscard]] const QDateTime &lastUpdated() const;

private:
    QString m_name;
    QString m_projectRoot;
    QString m_buildDirectory;
    QString m_version;
    QString m_changelog;
    QStringList m_uploadTargets;
    QDateTime m_lastUpdated;
};
