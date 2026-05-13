#include "ui/widgets/LogPanel.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace l10n = localization;

LogPanel::LogPanel(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("logPanel");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    auto *headerLayout = new QHBoxLayout;
    headerLayout->setSpacing(10);

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("sectionTitle");
    headerLayout->addWidget(m_titleLabel, 1);

    m_clearButton = new QPushButton;
    m_clearButton->setObjectName("secondaryButton");
    m_clearButton->setCursor(Qt::PointingHandCursor);
    connect(m_clearButton, &QPushButton::clicked, this, &LogPanel::clearMessages);
    headerLayout->addWidget(m_clearButton);

    layout->addLayout(headerLayout);

    m_output = new QPlainTextEdit;
    m_output->setObjectName("logOutput");
    m_output->setReadOnly(true);
    m_output->setLineWrapMode(QPlainTextEdit::NoWrap);
    layout->addWidget(m_output, 1);

    updateTexts();
}

void LogPanel::appendMessage(const QString &message)
{
    const auto timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    m_output->appendPlainText(QStringLiteral("[%1] %2").arg(timestamp, message));
}

void LogPanel::clearMessages()
{
    m_output->clear();
}

void LogPanel::setLanguage(localization::Language language)
{
    m_language = language;
    updateTexts();
}

void LogPanel::updateTexts()
{
    m_titleLabel->setText(l10n::translate(m_language, l10n::Text::LogOutput));
    m_clearButton->setText(l10n::translate(m_language, l10n::Text::Clear));
}
