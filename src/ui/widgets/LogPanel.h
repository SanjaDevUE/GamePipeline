#pragma once

#include "localization/Language.h"

#include <QWidget>

class QLabel;
class QPlainTextEdit;
class QPushButton;

class LogPanel final : public QWidget
{
    Q_OBJECT

public:
    explicit LogPanel(QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

public slots:
    void appendMessage(const QString &message);
    void clearMessages();

private:
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    QLabel *m_titleLabel = nullptr;
    QPushButton *m_clearButton = nullptr;
    QPlainTextEdit *m_output = nullptr;
};
