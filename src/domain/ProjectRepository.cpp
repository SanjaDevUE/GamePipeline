#include "domain/ProjectRepository.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>

#include <utility>

namespace {
QString projectsFilePath()
{
    auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (appDataPath.isEmpty()) {
        appDataPath = QDir::home().filePath(".gamepipeline");
    }

    return QDir(appDataPath).filePath("projects.json");
}

QStringList stringListFromJson(const QJsonArray &array)
{
    QStringList values;
    values.reserve(array.size());
    for (const auto &value : array) {
        values.append(value.toString());
    }

    return values;
}

QJsonArray stringListToJson(const QStringList &values)
{
    QJsonArray array;
    for (const auto &value : values) {
        array.append(value);
    }

    return array;
}

QVector<SteamDepotConfig> steamDepotsFromJson(const QJsonArray &array)
{
    QVector<SteamDepotConfig> depots;
    depots.reserve(array.size());
    for (const auto &value : array) {
        const auto object = value.toObject();
        SteamDepotConfig depot;
        depot.depotId = object.value("depotId").toString();
        depot.buildPath = object.value("buildPath").toString();
        if (!depot.depotId.isEmpty() || !depot.buildPath.isEmpty()) {
            depots.append(depot);
        }
    }

    return depots;
}

QJsonArray steamDepotsToJson(const QVector<SteamDepotConfig> &depots)
{
    QJsonArray array;
    for (const auto &depot : depots) {
        QJsonObject object;
        object.insert("depotId", depot.depotId);
        object.insert("buildPath", depot.buildPath);
        array.append(object);
    }

    return array;
}

GameProject projectFromJson(const QJsonObject &object)
{
    GameProject project;
    project.setName(object.value("name").toString());
    project.setProjectRoot(object.value("projectRoot").toString());
    project.setBuildDirectory(object.value("buildDirectory").toString());
    project.setVersion(object.value("version").toString());
    project.setChangelog(object.value("changelog").toString());
    project.setUploadTargets(stringListFromJson(object.value("uploadTargets").toArray()));

    auto lastUpdated = QDateTime::fromString(object.value("lastUpdated").toString(), Qt::ISODate);
    if (!lastUpdated.isValid()) {
        lastUpdated = QDateTime::currentDateTime();
    }
    project.setLastUpdated(lastUpdated);

    const auto steam = object.value("steam").toObject();
    project.setSteamAppId(steam.value("appId").toString());
    project.setSteamBuildDescription(steam.value("buildDescription").toString());
    project.setSteamDepots(steamDepotsFromJson(steam.value("depots").toArray()));
    project.setSteamContentBuilderPath(steam.value("contentBuilderPath").toString());
    project.setSteamBranch(steam.value("branch").toString());
    project.setSteamLogin(steam.value("login").toString());
    project.setSteamSavePassword(steam.value("savePassword").toBool(false));
    project.setSteamPassword(project.steamSavePassword() ? steam.value("password").toString() : QString{});
    project.setSteamPreviewBuild(steam.value("previewBuild").toBool(false));

    return project;
}

QJsonObject projectToJson(const GameProject &project)
{
    QJsonObject object;
    object.insert("name", project.name());
    object.insert("projectRoot", project.projectRoot());
    object.insert("buildDirectory", project.buildDirectory());
    object.insert("version", project.version());
    object.insert("changelog", project.changelog());
    object.insert("uploadTargets", stringListToJson(project.uploadTargets()));
    object.insert("lastUpdated", project.lastUpdated().toString(Qt::ISODate));

    QJsonObject steam;
    steam.insert("appId", project.steamAppId());
    steam.insert("buildDescription", project.steamBuildDescription());
    steam.insert("depots", steamDepotsToJson(project.steamDepots()));
    steam.insert("contentBuilderPath", project.steamContentBuilderPath());
    steam.insert("branch", project.steamBranch());
    steam.insert("login", project.steamLogin());
    steam.insert("savePassword", project.steamSavePassword());
    steam.insert("password", project.steamSavePassword() ? project.steamPassword() : QString{});
    steam.insert("previewBuild", project.steamPreviewBuild());
    object.insert("steam", steam);

    return object;
}
}

ProjectRepository::ProjectRepository(QObject *parent)
    : QObject(parent),
      m_storagePath(projectsFilePath())
{
    load();
}

const QVector<GameProject> &ProjectRepository::projects() const
{
    return m_projects;
}

QString ProjectRepository::storagePath() const
{
    return m_storagePath;
}

int ProjectRepository::addProject(const GameProject &project)
{
    m_projects.append(project);
    save();
    emit projectsChanged();
    return m_projects.size() - 1;
}

void ProjectRepository::updateProject(int index, const GameProject &project)
{
    if (index < 0 || index >= m_projects.size()) {
        return;
    }

    m_projects[index] = project;
    save();
    emit projectsChanged();
}

void ProjectRepository::removeProject(int index)
{
    if (index < 0 || index >= m_projects.size()) {
        return;
    }

    m_projects.removeAt(index);
    save();
    emit projectsChanged();
}

bool ProjectRepository::load(QString *errorMessage)
{
    QFile file(m_storagePath);
    if (!file.exists()) {
        m_projects.clear();
        return true;
    }

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    const auto document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Invalid project file format.");
        }
        return false;
    }

    QVector<GameProject> loadedProjects;
    const auto projects = document.object().value("projects").toArray();
    loadedProjects.reserve(projects.size());
    for (const auto &value : projects) {
        const auto project = projectFromJson(value.toObject());
        if (!project.name().isEmpty()) {
            loadedProjects.append(project);
        }
    }

    m_projects = std::move(loadedProjects);
    return true;
}

bool ProjectRepository::save(QString *errorMessage) const
{
    const QFileInfo fileInfo(m_storagePath);
    if (!QDir().mkpath(fileInfo.absolutePath())) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Could not create project storage folder.");
        }
        return false;
    }

    QJsonArray projects;
    for (const auto &project : m_projects) {
        projects.append(projectToJson(project));
    }

    QJsonObject root;
    root.insert("projects", projects);

    QSaveFile file(m_storagePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    return true;
}
