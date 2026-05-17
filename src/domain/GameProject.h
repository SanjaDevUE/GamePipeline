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
    [[nodiscard]] const QString &unrealEnginePath() const;
    [[nodiscard]] const QString &unrealProjectPath() const;
    [[nodiscard]] const QString &unrealArchiveDirectory() const;
    [[nodiscard]] const QString &unrealPlatform() const;
    [[nodiscard]] const QString &unrealConfiguration() const;
    [[nodiscard]] const QString &unrealAdditionalArguments() const;
    [[nodiscard]] bool unrealBuild() const;
    [[nodiscard]] bool unrealCook() const;
    [[nodiscard]] bool unrealStage() const;
    [[nodiscard]] bool unrealPak() const;
    [[nodiscard]] bool unrealArchive() const;
    [[nodiscard]] bool unrealClean() const;
    [[nodiscard]] bool unrealDistribution() const;

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
    void setUnrealEnginePath(QString unrealEnginePath);
    void setUnrealProjectPath(QString unrealProjectPath);
    void setUnrealArchiveDirectory(QString unrealArchiveDirectory);
    void setUnrealPlatform(QString unrealPlatform);
    void setUnrealConfiguration(QString unrealConfiguration);
    void setUnrealAdditionalArguments(QString unrealAdditionalArguments);
    void setUnrealBuild(bool unrealBuild);
    void setUnrealCook(bool unrealCook);
    void setUnrealStage(bool unrealStage);
    void setUnrealPak(bool unrealPak);
    void setUnrealArchive(bool unrealArchive);
    void setUnrealClean(bool unrealClean);
    void setUnrealDistribution(bool unrealDistribution);

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
    QString m_unrealEnginePath;
    QString m_unrealProjectPath;
    QString m_unrealArchiveDirectory;
    QString m_unrealPlatform = QStringLiteral("Win64");
    QString m_unrealConfiguration = QStringLiteral("Development");
    QString m_unrealAdditionalArguments;
    bool m_unrealBuild = true;
    bool m_unrealCook = true;
    bool m_unrealStage = true;
    bool m_unrealPak = true;
    bool m_unrealArchive = true;
    bool m_unrealClean = false;
    bool m_unrealDistribution = false;
};
