#include "ui/MainWindow.h"

#include <QApplication>
#include <QFile>
#include <QIcon>

namespace {
void applyApplicationStyle(QApplication &application)
{
    QFile styleSheet(":/styles/dark.qss");
    if (styleSheet.open(QFile::ReadOnly | QFile::Text)) {
        application.setStyleSheet(QString::fromUtf8(styleSheet.readAll()));
    }
}
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName("GamePipeline");
    QApplication::setOrganizationName("GamePipeline");
    QApplication::setApplicationVersion(GAMEPIPELINE_VERSION);
    QApplication::setWindowIcon(QIcon(":/icons/app.ico"));

    applyApplicationStyle(application);

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
