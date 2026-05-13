#include "ui/pages/LogsPage.h"

#include "ui/widgets/LogPanel.h"

#include <QLabel>
#include <QVBoxLayout>

namespace l10n = localization;

LogsPage::LogsPage(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("page");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 26, 28, 28);
    layout->setSpacing(18);

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("pageTitle");
    layout->addWidget(m_titleLabel);

    m_logPanel = new LogPanel(this);
    layout->addWidget(m_logPanel, 1);

    updateTexts();
}

void LogsPage::appendMessage(const QString &message)
{
    m_logPanel->appendMessage(message);
}

void LogsPage::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void LogsPage::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::LogsTitle));
    m_logPanel->setLanguage(m_language);
}
