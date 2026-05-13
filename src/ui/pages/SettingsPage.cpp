#include "ui/pages/SettingsPage.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>

namespace l10n = localization;

namespace {
QLineEdit *createInput(const QString &placeholder)
{
    auto *input = new QLineEdit;
    input->setPlaceholderText(placeholder);
    return input;
}
}

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("page");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 26, 28, 28);
    layout->setSpacing(18);

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("pageTitle");
    layout->addWidget(m_titleLabel);

    auto *panel = new QWidget;
    panel->setObjectName("panel");
    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 20, 20, 20);
    panelLayout->setSpacing(16);

    m_sectionTitleLabel = new QLabel;
    m_sectionTitleLabel->setObjectName("sectionTitle");
    panelLayout->addWidget(m_sectionTitleLabel);

    auto *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);
    form->setHorizontalSpacing(14);
    form->setVerticalSpacing(12);
    m_workspaceFolderLabel = new QLabel;
    m_steamCmdPathLabel = new QLabel;
    m_butlerPathLabel = new QLabel;
    form->addRow(m_workspaceFolderLabel, createInput("D:/GameBuilds"));
    form->addRow(m_steamCmdPathLabel, createInput("C:/steamcmd/steamcmd.exe"));
    form->addRow(m_butlerPathLabel, createInput("C:/Tools/butler.exe"));

    m_languageLabel = new QLabel;
    m_languageComboBox = new QComboBox;
    form->addRow(m_languageLabel, m_languageComboBox);

    m_themeLabel = new QLabel;
    m_darkThemeCheckBox = new QCheckBox;
    m_darkThemeCheckBox->setChecked(true);
    form->addRow(m_themeLabel, m_darkThemeCheckBox);
    panelLayout->addLayout(form);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    m_saveButton = new QPushButton;
    m_saveButton->setObjectName("primaryButton");
    m_saveButton->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(m_saveButton);
    panelLayout->addLayout(buttonLayout);

    connect(m_saveButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSettingsSave));
    });
    connect(m_languageComboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (index < 0) {
            return;
        }

        const auto selectedLanguage = static_cast<l10n::Language>(m_languageComboBox->currentData().toInt());
        if (selectedLanguage != m_language) {
            emit languageChanged(selectedLanguage);
        }
    });

    layout->addWidget(panel);
    layout->addStretch(1);

    updateTexts();
}

void SettingsPage::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void SettingsPage::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::SettingsTitle));
    m_sectionTitleLabel->setText(l10n::translate(m_language, l10n::Text::ApplicationSettings));
    m_workspaceFolderLabel->setText(l10n::translate(m_language, l10n::Text::WorkspaceFolder));
    m_steamCmdPathLabel->setText(l10n::translate(m_language, l10n::Text::SteamCmdPath));
    m_butlerPathLabel->setText(l10n::translate(m_language, l10n::Text::ButlerPath));
    m_languageLabel->setText(l10n::translate(m_language, l10n::Text::Language));
    m_themeLabel->setText(l10n::translate(m_language, l10n::Text::Theme));
    m_darkThemeCheckBox->setText(l10n::translate(m_language, l10n::Text::UseDarkTheme));
    m_saveButton->setText(l10n::translate(m_language, l10n::Text::SaveSettings));

    const QSignalBlocker blocker(m_languageComboBox);
    m_languageComboBox->clear();
    m_languageComboBox->addItem(l10n::languageName(l10n::Language::English, m_language),
                                static_cast<int>(l10n::Language::English));
    m_languageComboBox->addItem(l10n::languageName(l10n::Language::German, m_language),
                                static_cast<int>(l10n::Language::German));
    m_languageComboBox->setCurrentIndex(m_language == l10n::Language::English ? 0 : 1);
}
