#include "log/log.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "platform/platform.h"

#include <QApplication>
#include <QDebug>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("pump"));
    QApplication::setOrganizationName(QStringLiteral("cxth"));

    Log::init();
    Log::writeBootBanner();
    PictureManager::instance().loadAll();

    QFont font = app.font();
    font.setPointSize(8);
    app.setFont(font);

    MainWindow w;
    if (Platform *platform = createPlatform())
    {
        platform->applyWindowMode(&w);
        delete platform;
    }
    const int rc = app.exec();
    qInfo() << "application exiting, code=" << rc;
    Log::shutdown();
    return rc;
}
