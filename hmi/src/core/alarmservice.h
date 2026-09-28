#ifndef ALARMSERVICE_H
#define ALARMSERVICE_H

#include <QObject>
#include <QSet>

/** Pressure / comm / weeping alarms (weiduodianzi MachineStat::sysError). */
class AlarmService : public QObject
{
    Q_OBJECT
public:
    enum Kind {
        NoWarn = 0,
        CommunicationErr = 1,
        OverpressErr = 3,
        Weeping = 4
    };

    explicit AlarmService(QObject *parent = nullptr);

    void setAlarm(Kind kind, bool active);
    void clearAll();
    Kind primaryAlarm() const;
    double overPressure() const { return m_overPress; }

    /** Returns 0=none, 1=low, 2=high for BottomBar. */
    int pressWarnLevel() const;

    void checkPressure(double mpa, quint32 runSec, double pmin, double pmax);

signals:
    void alarmChanged();

private:
    QSet<int> m_active;
    double m_overPress = 0;
    int m_pressWarn = 0;
};

#endif
