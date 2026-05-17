#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QVector>

struct SteamDepotConfig
{
    QString depotId;
    QString buildPath;
};

class GameProject
{
public:
    GameProject() = default;
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
    [[nodiscard]] const QString &steamAppId() const;
    [[nodiscard]] const QString &steamBuildDescription() const;
    [[nodiscard]] const QVector<SteamDepotConfig> &steamDepots() const;
    [[nodiscard]] const QString &steamContentBuilderPath() const;
    [[nodiscard]] const QString &steamBranch() const;
    [[nodiscard]] const QString &steamLogin() const;
    [[nodiscard]] const QString &steamPassword() const;
    [[nodiscard]] bool steamSavePassword() const;
    [[nodiscard]] bool steamPreviewBuild() const;

    void setName(QString name);
    void setProjectRoot(QString projectRoot);
    void setBuildDirectory(QString buildDirectory);
    void setVersion(QString version);
    void setChangelog(QString changelog);
    void setUploadTargets(QStringList uploadTargets);
    void setLastUpdated(QDateTime lastUpdated);
    void setSteamAppId(QString steamAppId);
    void setSteamBuildDescription(QString steamBuildDescription);
    void setSteamDepots(QVector<SteamDepotConfig> steamDepots);
    void setSteamContentBuilderPath(QString steamContentBuilderPath);
    void setSteamBranch(QString steamBranch);
    void setSteamLogin(QString steamLogin);
    void setSteamPassword(QString steamPassword);
    void setSteamSavePassword(bool steamSavePassword);
    void setSteamPreviewBuild(bool steamPreviewBuild);

private:
    QString m_name;
    QString m_projectRoot;
    QString m_buildDirectory;
    QString m_version;
    QString m_changelog;
    QStringList m_uploadTargets;
    QDateTime m_lastUpdated;
    QString m_steamAppId;
    QString m_steamBuildDescription;
    QVector<SteamDepotConfig> m_steamDepots;
    QString m_steamContentBuilderPath;
    QString m_steamBranch;
    QString m_steamLogin;
    QString m_steamPassword;
    bool m_steamSavePassword = false;
    bool m_steamPreviewBuild = false;
};
