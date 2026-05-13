#include "ui/pages/ItchPage.h"

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

ItchPage::ItchPage(QWidget *parent)
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
    m_butlerPathLabel = new QLabel;
    m_userGameLabel = new QLabel;
    m_channelLabel = new QLabel;
    m_versionLabel = new QLabel;
    form->addRow(m_butlerPathLabel, createInput("C:/Tools/butler.exe"));
    form->addRow(m_userGameLabel, createInput("username/game-name"));
    form->addRow(m_channelLabel, createInput("windows"));
    form->addRow(m_versionLabel, createInput("0.1.0"));
    panelLayout->addLayout(form);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(12);
    m_validateButton = createActionButton({});
    m_statusButton = createActionButton({});
    m_uploadButton = createActionButton({}, "accentButton");
    buttonLayout->addWidget(m_validateButton);
    buttonLayout->addWidget(m_statusButton);
    buttonLayout->addWidget(m_uploadButton);
    buttonLayout->addStretch(1);
    panelLayout->addLayout(buttonLayout);

    connect(m_validateButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogItchValidate));
    });
    connect(m_statusButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogItchStatus));
    });
    connect(m_uploadButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogItchUpload));
    });

    layout->addWidget(panel);
    layout->addStretch(1);

    updateTexts();
}

void ItchPage::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void ItchPage::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::ItchTitle));
    m_sectionTitleLabel->setText(l10n::translate(m_language, l10n::Text::ButlerPlaceholder));
    m_butlerPathLabel->setText(l10n::translate(m_language, l10n::Text::ButlerPath));
    m_userGameLabel->setText(l10n::translate(m_language, l10n::Text::UserGame));
    m_channelLabel->setText(l10n::translate(m_language, l10n::Text::Channel));
    m_versionLabel->setText(l10n::translate(m_language, l10n::Text::Version));
    m_validateButton->setText(l10n::translate(m_language, l10n::Text::ValidateConfig));
    m_statusButton->setText(l10n::translate(m_language, l10n::Text::CheckStatus));
    m_uploadButton->setText(l10n::translate(m_language, l10n::Text::PushWithButler));
}
