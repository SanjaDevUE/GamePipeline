#include "ui/MainWindow.h"

#include "domain/ProjectRepository.h"
#include "ui/pages/DashboardPage.h"
#include "ui/pages/ItchPage.h"
#include "ui/pages/LogsPage.h"
#include "ui/pages/ProjectsPage.h"
#include "ui/pages/SettingsPage.h"
#include "ui/pages/SteamPage.h"

#include <QStatusBar>
#include <QTabWidget>

namespace l10n = localization;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    connectPageLogs();
    setLanguage(l10n::Language::English);
    writeLog(l10n::translate(m_language, l10n::Text::AppStarted));
}

void MainWindow::setupUi()
{
    setWindowTitle("GamePipeline");
    resize(1280, 880);
    setMinimumSize(1100, 740);

    m_tabs = new QTabWidget(this);
    m_tabs->setObjectName("mainTabs");
    m_tabs->setDocumentMode(true);

    m_projectRepository = new ProjectRepository(this);
    m_dashboardPage = new DashboardPage(this);
    m_projectsPage = new ProjectsPage(m_projectRepository, this);
    m_steamPage = new SteamPage(m_projectRepository, this);
    m_itchPage = new ItchPage(this);
    m_logsPage = new LogsPage(this);
    m_settingsPage = new SettingsPage(this);

    connect(m_projectRepository, &ProjectRepository::projectsChanged, this, [this] {
        m_dashboardPage->setProjectCount(m_projectRepository->projects().size());
    });
    m_dashboardPage->setProjectCount(m_projectRepository->projects().size());

    m_tabs->addTab(m_dashboardPage, {});
    m_tabs->addTab(m_projectsPage, {});
    m_tabs->addTab(m_steamPage, {});
    m_tabs->addTab(m_itchPage, {});
    m_tabs->addTab(m_logsPage, {});
    m_tabs->addTab(m_settingsPage, {});

    setCentralWidget(m_tabs);
}

void MainWindow::connectPageLogs()
{
    connect(m_dashboardPage, &DashboardPage::logRequested, this, &MainWindow::writeLog);
    connect(m_projectsPage, &ProjectsPage::logRequested, this, &MainWindow::writeLog);
    connect(m_steamPage, &SteamPage::logRequested, this, &MainWindow::writeLog);
    connect(m_itchPage, &ItchPage::logRequested, this, &MainWindow::writeLog);
    connect(m_settingsPage, &SettingsPage::logRequested, this, &MainWindow::writeLog);
    connect(m_settingsPage, &SettingsPage::languageChanged, this, &MainWindow::setLanguage);
}

void MainWindow::writeLog(const QString &message)
{
    if (m_logsPage) {
        m_logsPage->appendMessage(message);
    }

    statusBar()->showMessage(message, 4000);
}

void MainWindow::setLanguage(localization::Language language)
{
    const bool changed = m_language != language;
    m_language = language;
    updateTexts();

    if (changed) {
        writeLog(l10n::translate(m_language, l10n::Text::LanguageChanged)
                     .arg(l10n::languageName(m_language, m_language)));
    }
}

void MainWindow::updateTexts()
{
    m_tabs->setTabText(0, l10n::translate(m_language, l10n::Text::TabDashboard));
    m_tabs->setTabText(1, l10n::translate(m_language, l10n::Text::TabProjects));
    m_tabs->setTabText(2, l10n::translate(m_language, l10n::Text::TabSteam));
    m_tabs->setTabText(3, l10n::translate(m_language, l10n::Text::TabItch));
    m_tabs->setTabText(4, l10n::translate(m_language, l10n::Text::TabLogs));
    m_tabs->setTabText(5, l10n::translate(m_language, l10n::Text::TabSettings));

    m_dashboardPage->setLanguage(m_language);
    m_projectsPage->setLanguage(m_language);
    m_steamPage->setLanguage(m_language);
    m_itchPage->setLanguage(m_language);
    m_logsPage->setLanguage(m_language);
    m_settingsPage->setLanguage(m_language);

    statusBar()->showMessage(l10n::translate(m_language, l10n::Text::Ready));
}
