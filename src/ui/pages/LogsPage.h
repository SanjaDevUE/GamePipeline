#pragma once

#include "localization/Language.h"

#include <QWidget>

class LogPanel;
class QLabel;

class LogsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit LogsPage(QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

public slots:
    void appendMessage(const QString &message);

private:
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    QLabel *m_titleLabel = nullptr;
    LogPanel *m_logPanel = nullptr;
};
