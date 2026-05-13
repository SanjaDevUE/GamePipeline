#pragma once

#include "localization/Language.h"

#include <QWidget>

class QLabel;
class QPushButton;

class SteamPage final : public QWidget
{
    Q_OBJECT

public:
    explicit SteamPage(QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

signals:
    void logRequested(const QString &message);

private:
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_sectionTitleLabel = nullptr;
    QLabel *m_steamCmdPathLabel = nullptr;
    QLabel *m_appIdLabel = nullptr;
    QLabel *m_depotIdLabel = nullptr;
    QLabel *m_buildDescriptionLabel = nullptr;
    QPushButton *m_validateButton = nullptr;
    QPushButton *m_generateButton = nullptr;
    QPushButton *m_uploadButton = nullptr;
};
