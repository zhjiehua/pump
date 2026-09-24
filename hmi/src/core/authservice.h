#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <QObject>

class AppSettings;

/** Licensing / activation (weiduodianzi MachineStat::generateActiveCode). */
class AuthService : public QObject
{
    Q_OBJECT
public:
    explicit AuthService(AppSettings *settings, QObject *parent = nullptr);

    quint64 generateActiveCode(quint64 serialNum, quint8 which) const;
    quint32 tryDaysFromActiveCode(quint64 activeNum, quint32 serialId) const;
    void ensureSerial();
    bool activate(quint64 activeNum, bool permanentCheck = true);
    bool checkProbationExpired() const;

signals:
    void authChanged();

private:
    AppSettings *m_settings = nullptr;
};

#endif
