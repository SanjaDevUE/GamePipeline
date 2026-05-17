#include "ui/MainWindow.h"

#include "domain/ProjectRepository.h"
#include "ui/pages/LogsPage.h"
#include "ui/pages/SteamPage.h"

#include <QSplitter>
#include <QStatusBar>

namespace l10n = localization;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    connectPageLogs();
    updateTexts();
    writeLog(l10n::translate(m_language, l10n::Text::AppStarted));
}

void MainWindow::setupUi()
{
    setWindowTitle("GamePipeline Steam Uploader");
    resize(1280, 920);
    setMinimumSize(1120, 780);

    m_projectRepository = new ProjectRepository(this);
    m_steamPage = new SteamPage(m_projectRepository, this);
    m_logsPage = new LogsPage(this);

    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->setObjectName("mainSplitter");
    m_splitter->addWidget(m_steamPage);
    m_splitter->addWidget(m_logsPage);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);
    m_splitter->setStretchFactor(0, 5);
    m_splitter->setStretchFactor(1, 2);
    m_splitter->setSizes({620, 260});

    setCentralWidget(m_splitter);
}

void MainWindow::connectPageLogs()
{
    connect(m_steamPage, &SteamPage::logRequested, this, &MainWindow::writeLog);
}

void MainWindow::writeLog(const QString &message)
{
    if (m_logsPage) {
        m_logsPage->appendMessage(message);
    }

    statusBar()->showMessage(message, 4000);
}

void MainWindow::updateTexts()
{
    m_steamPage->setLanguage(m_language);
    m_logsPage->setLanguage(m_language);
    statusBar()->showMessage(l10n::translate(m_language, l10n::Text::Ready));
}
