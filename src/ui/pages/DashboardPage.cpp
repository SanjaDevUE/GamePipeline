#include "ui/pages/DashboardPage.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace l10n = localization;

namespace {
QWidget *createMetricCard(QLabel *&label, QLabel *&value, const QString &initialValue)
{
    auto *card = new QWidget;
    card->setObjectName("metricCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(8);

    value = new QLabel(initialValue);
    value->setObjectName("metricValue");
    layout->addWidget(value);

    label = new QLabel;
    label->setObjectName("mutedText");
    layout->addWidget(label);

    return card;
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

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("page");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 26, 28, 28);
    layout->setSpacing(20);

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("pageTitle");
    layout->addWidget(m_titleLabel);

    auto *summaryGrid = new QGridLayout;
    summaryGrid->setHorizontalSpacing(14);
    summaryGrid->setVerticalSpacing(14);
    QLabel *managedProjectsValue = nullptr;
    QLabel *readyBuildsValue = nullptr;
    summaryGrid->addWidget(createMetricCard(m_managedProjectsLabel, managedProjectsValue, "2"), 0, 0);
    summaryGrid->addWidget(createMetricCard(m_readyBuildsLabel, readyBuildsValue, "0"), 0, 1);
    summaryGrid->addWidget(createMetricCard(m_steamUploadsLabel, m_steamPlaceholderLabel, {}), 0, 2);
    summaryGrid->addWidget(createMetricCard(m_itchUploadsLabel, m_itchPlaceholderLabel, {}), 0, 3);
    layout->addLayout(summaryGrid);

    auto *panel = new QWidget;
    panel->setObjectName("panel");
    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 20, 20, 20);
    panelLayout->setSpacing(14);

    m_quickActionsLabel = new QLabel;
    m_quickActionsLabel->setObjectName("sectionTitle");
    panelLayout->addWidget(m_quickActionsLabel);

    auto *buttons = new QHBoxLayout;
    buttons->setSpacing(12);
    m_newProjectButton = createActionButton({}, "primaryButton");
    m_selectBuildButton = createActionButton({});
    m_steamButton = createActionButton({});
    m_itchButton = createActionButton({});
    buttons->addWidget(m_newProjectButton);
    buttons->addWidget(m_selectBuildButton);
    buttons->addWidget(m_steamButton);
    buttons->addWidget(m_itchButton);
    buttons->addStretch(1);
    panelLayout->addLayout(buttons);

    connect(m_newProjectButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogDashboardNewProject));
    });
    connect(m_selectBuildButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogDashboardBuildFolder));
    });
    connect(m_steamButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogDashboardSteamUpload));
    });
    connect(m_itchButton, &QPushButton::clicked, this, [this] {
        emit logRequested(l10n::translate(m_language, l10n::Text::LogDashboardItchUpload));
    });

    layout->addWidget(panel);
    layout->addStretch(1);

    updateTexts();
}

void DashboardPage::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void DashboardPage::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::DashboardTitle));
    m_managedProjectsLabel->setText(l10n::translate(m_language, l10n::Text::ManagedProjects));
    m_readyBuildsLabel->setText(l10n::translate(m_language, l10n::Text::ReadyBuilds));
    m_steamUploadsLabel->setText(l10n::translate(m_language, l10n::Text::SteamUploads));
    m_itchUploadsLabel->setText(l10n::translate(m_language, l10n::Text::ItchUploads));
    m_steamPlaceholderLabel->setText(l10n::translate(m_language, l10n::Text::Placeholder));
    m_itchPlaceholderLabel->setText(l10n::translate(m_language, l10n::Text::Placeholder));
    m_quickActionsLabel->setText(l10n::translate(m_language, l10n::Text::QuickActions));
    m_newProjectButton->setText(l10n::translate(m_language, l10n::Text::NewProject));
    m_selectBuildButton->setText(l10n::translate(m_language, l10n::Text::SelectBuildFolder));
    m_steamButton->setText(l10n::translate(m_language, l10n::Text::PrepareSteamUpload));
    m_itchButton->setText(l10n::translate(m_language, l10n::Text::PrepareItchUpload));
}
