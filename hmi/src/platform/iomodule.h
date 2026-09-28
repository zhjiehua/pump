#ifndef IOMODULE_H
#define IOMODULE_H

#include <QObject>

/** Linux /dev/pwm GPIO (embedded); no-op on desktop. */
class IoModule : public QObject
{
    Q_OBJECT
public:
    static IoModule *instance();

    void initHardware();
    void logicSetIo(quint32 mask, bool value);
    bool logicGetIo(quint32 mask) const;
    void doWarn(bool on);

signals:
    void bulge();
    void weeping(bool flag);

private slots:
    void onWarnTimeout();

private:
    explicit IoModule(QObject *parent = nullptr);
    int m_fd;
};

#endif
