#pragma once

#include "localization/Language.h"

#include <QWidget>

class QLabel;
class QPushButton;

class DashboardPage final : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

signals:
    void logRequested(const QString &message);

private:
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_managedProjectsLabel = nullptr;
    QLabel *m_readyBuildsLabel = nullptr;
    QLabel *m_steamUploadsLabel = nullptr;
    QLabel *m_itchUploadsLabel = nullptr;
    QLabel *m_steamPlaceholderLabel = nullptr;
    QLabel *m_itchPlaceholderLabel = nullptr;
    QLabel *m_quickActionsLabel = nullptr;
    QPushButton *m_newProjectButton = nullptr;
    QPushButton *m_selectBuildButton = nullptr;
    QPushButton *m_steamButton = nullptr;
    QPushButton *m_itchButton = nullptr;
};
