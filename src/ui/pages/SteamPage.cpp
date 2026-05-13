#include "ui/pages/SteamPage.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace l10n = localization;

namespace {
QLineEdit *createInput(const QString &placeholder)
{
    auto *input = new QLineEdit;
    input->setPlaceholderText(placeholder);
    return input;
}

QPushButton *createActionButton(const QString &text, const QString &objectName = {})
{
    auto *button = new QPushButton(text);
    button->setCursor(Qt::PointingHandCursor);
    if (!objectName.isEmpty()) {
        button->setObjectName(objectName);
    }
    return button;
}
}

SteamPage::SteamPage(QWidget *parent)
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
    m_steamCmdPathLabel = new QLabel;
    m_appIdLabel = new QLabel;
    m_depotIdLabel = new QLabel;
    m_buildDescriptionLabel = new QLabel;
    form->addRow(m_steamCmdPathLabel, createInput("C:/steamcmd/steamcmd.exe"));
    form->addRow(m_appIdLabel, createInput("480"));
    form->addRow(m_depotIdLabel, createInput("1001"));
    form->addRow(m_buildDescriptionLabel, createInput("Windows build 0.1.0"));
    panelLayout->addLayout(form);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(12);
    m_validateButton = createActionButton({});
    m_generateButton = createActionButton({});
    m_uploadButton = createActionButton({}, "primaryButton");
    buttonLayout->addWidget(m_validateButton);
    buttonLayout->addWidget(m_generateButton);
    buttonLayout->addWidget(m_uploadButton);
    buttonLayout->addStretch(1);
    panelLayout->addLayout(buttonLayout);

    connect(m_validateButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamValidate));
    });
    connect(m_generateButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamGenerateVdf));
    });
    connect(m_uploadButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogSteamUpload));
    });

    layout->addWidget(panel);
    layout->addStretch(1);

    updateTexts();
}

void SteamPage::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void SteamPage::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::SteamTitle));
    m_sectionTitleLabel->setText(l10n::translate(m_language, l10n::Text::SteamCmdPlaceholder));
    m_steamCmdPathLabel->setText(l10n::translate(m_language, l10n::Text::SteamCmdPath));
    m_appIdLabel->setText(l10n::translate(m_language, l10n::Text::AppId));
    m_depotIdLabel->setText(l10n::translate(m_language, l10n::Text::DepotId));
    m_buildDescriptionLabel->setText(l10n::translate(m_language, l10n::Text::BuildDescription));
    m_validateButton->setText(l10n::translate(m_language, l10n::Text::ValidateConfig));
    m_generateButton->setText(l10n::translate(m_language, l10n::Text::GenerateVdf));
    m_uploadButton->setText(l10n::translate(m_language, l10n::Text::UploadToSteam));
}
