#include "ui/MainWindow.h"

#include "domain/ProjectRepository.h"
#include "ui/pages/LogsPage.h"
#include "ui/pages/ProjectsPage.h"
#include "ui/pages/SteamPage.h"

#include <QStatusBar>
#include <QTabWidget>

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

    m_tabs = new QTabWidget(this);
    m_tabs->setObjectName("mainTabs");
    m_tabs->setDocumentMode(true);

    m_projectRepository = new ProjectRepository(this);
    m_projectsPage = new ProjectsPage(m_projectRepository, this);
    m_steamPage = new SteamPage(m_projectRepository, this);
    m_logsPage = new LogsPage(this);

    m_tabs->addTab(m_projectsPage, {});
    m_tabs->addTab(m_steamPage, {});
    m_tabs->addTab(m_logsPage, {});

    setCentralWidget(m_tabs);
}

void MainWindow::connectPageLogs()
{
    connect(m_projectsPage, &ProjectsPage::logRequested, this, &MainWindow::writeLog);
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
    m_tabs->setTabText(0, l10n::translate(m_language, l10n::Text::TabProjects));
    m_tabs->setTabText(1, l10n::translate(m_language, l10n::Text::TabSteam));
    m_tabs->setTabText(2, l10n::translate(m_language, l10n::Text::TabLogs));

    m_projectsPage->setLanguage(m_language);
    m_steamPage->setLanguage(m_language);
    m_logsPage->setLanguage(m_language);
    statusBar()->showMessage(l10n::translate(m_language, l10n::Text::Ready));
}
