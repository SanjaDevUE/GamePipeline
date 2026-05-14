#pragma once

#include "domain/GameProject.h"

#include <QObject>
#include <QVector>

class ProjectRepository final : public QObject
{
    Q_OBJECT

public:
    explicit ProjectRepository(QObject *parent = nullptr);

    [[nodiscard]] const QVector<GameProject> &projects() const;
    [[nodiscard]] QString storagePath() const;

    int addProject(const GameProject &project);
    void updateProject(int index, const GameProject &project);
    void removeProject(int index);
    bool load(QString *errorMessage = nullptr);
    bool save(QString *errorMessage = nullptr) const;

signals:
    void projectsChanged();

private:
    QVector<GameProject> m_projects;
    QString m_storagePath;
};
