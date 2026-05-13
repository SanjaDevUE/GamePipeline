#pragma once

#include "localization/Language.h"

#include <QWidget>

class QCheckBox;
class QComboBox;
class QLabel;
class QPushButton;

class SettingsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    void setLanguage(localization::Language language);

signals:
    void logRequested(const QString &message);
    void languageChanged(localization::Language language);

private:
    void updateTexts();

    localization::Language m_language = localization::Language::English;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_sectionTitleLabel = nullptr;
    QLabel *m_workspaceFolderLabel = nullptr;
    QLabel *m_steamCmdPathLabel = nullptr;
    QLabel *m_butlerPathLabel = nullptr;
    QLabel *m_languageLabel = nullptr;
    QLabel *m_themeLabel = nullptr;
    QComboBox *m_languageComboBox = nullptr;
    QCheckBox *m_darkThemeCheckBox = nullptr;
    QPushButton *m_saveButton = nullptr;
};
