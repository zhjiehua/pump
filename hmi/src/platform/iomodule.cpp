#include "platform/iomodule.h"
#include "utils/hmiconfig.h"

#include <QTimer>

#if defined(__linux__) && HMI_EMBEDDED
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef IOC_MAGIC
#define IOC_MAGIC 'A'
#define PWM_IOCTL_SET_IO _IO(IOC_MAGIC, 5)
#define PWM_IOCTL_GET_IO _IO(IOC_MAGIC, 7)
#endif
#endif

IoModule *IoModule::instance()
{
    static IoModule s;
    return &s;
}

IoModule::IoModule(QObject *parent)
    : QObject(parent)
    , m_fd(-1)
{
}

void IoModule::initHardware()
{
#if defined(__linux__) && HMI_EMBEDDED
    m_fd = ::open("/dev/pwm", O_RDWR);
#endif
}

void IoModule::logicSetIo(quint32 mask, bool value)
{
#if defined(__linux__) && HMI_EMBEDDED
    if (m_fd < 0)
        return;
    unsigned long arg = mask;
    if (value)
        ::ioctl(m_fd, PWM_IOCTL_SET_IO, arg);
#else
    Q_UNUSED(mask);
    Q_UNUSED(value);
#endif
}

bool IoModule::logicGetIo(quint32 mask) const
{
#if defined(__linux__) && HMI_EMBEDDED
    if (m_fd < 0)
        return false;
    unsigned long arg = mask;
    const int v = ::ioctl(m_fd, PWM_IOCTL_GET_IO, arg);
    return v != 0;
#else
    Q_UNUSED(mask);
    return false;
#endif
}

void IoModule::doWarn(bool on)
{
    if (!on)
        return;
    QTimer::singleShot(3000, this, SLOT(onWarnTimeout()));
    logicSetIo(1u << 12, true);
}

void IoModule::onWarnTimeout()
{
    logicSetIo(1u << 12, false);
}
