#pragma once

#include "localization/Language.h"

#include <QWidget>

class QLabel;
class QPushButton;

class ItchPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ItchPage(QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

signals:
    void logRequested(const QString &message);

private:
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_sectionTitleLabel = nullptr;
    QLabel *m_butlerPathLabel = nullptr;
    QLabel *m_userGameLabel = nullptr;
    QLabel *m_channelLabel = nullptr;
    QLabel *m_versionLabel = nullptr;
    QPushButton *m_validateButton = nullptr;
    QPushButton *m_statusButton = nullptr;
    QPushButton *m_uploadButton = nullptr;
};
