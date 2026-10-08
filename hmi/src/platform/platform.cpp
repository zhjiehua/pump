#include "platform/platform.h"
#include "platform/iomodule.h"
#include "utils/hmiconfig.h"

#include <QApplication>
#include <QWidget>

#if HMI_EMBEDDED

class EmbeddedPlatform : public Platform
{
public:
    void applyWindowMode(QWidget *window) override
    {
        QApplication::setOverrideCursor(Qt::BlankCursor);
        window->setWindowFlags(Qt::FramelessWindowHint);
        IoModule::instance()->initHardware();
        window->showFullScreen();
    }
    bool isEmbedded() const override { return true; }
};

Platform *createPlatform()
{
    return new EmbeddedPlatform;
}

#else

class DesktopPlatform : public Platform
{
public:
    void applyWindowMode(QWidget *window) override { window->show(); }
    bool isEmbedded() const override { return false; }
};

Platform *createPlatform()
{
    return new DesktopPlatform;
}

#endif
