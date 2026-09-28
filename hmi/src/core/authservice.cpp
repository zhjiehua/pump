#include "core/authservice.h"
#include "core/appsettings.h"

#include <QDateTime>
#include <QtGlobal>

namespace {
constexpr quint64 kSerialMax = 9999999999ULL;
constexpr quint64 kActiveCodeTry = 9000000000ULL;
constexpr quint64 kActiveCodeMax = 999999999ULL;
constexpr quint64 kActiveMask = 0x12345678ULL;
constexpr quint64 kSerialNumKey = 19900208ULL;
constexpr quint64 kActiveCodeTrySuffix = 9000000000ULL;
} // namespace

AuthService::AuthService(AppSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
}

quint64 AuthService::generateActiveCode(quint64 serialNum, quint8 which) const
{
    quint64 ret = 0;
    const quint64 date = m_settings ? m_settings->serialId : 0;
    if (which == 0)
    {
        ret = (serialNum + date) % kActiveCodeMax;
        ret ^= kActiveMask;
        ret %= kActiveCodeMax;
    }
    else
    {
        ret = (serialNum + date) % kActiveCodeMax;
        ret ^= kActiveMask;
        ret %= kActiveCodeMax;
        ret /= 1000;
        ret *= 1000;
        ret += kActiveCodeTrySuffix;
    }
    return ret;
}

quint32 AuthService::tryDaysFromActiveCode(quint64 activeNum, quint32 serialId) const
{
    quint32 tryday = quint32(activeNum % 1000);
    serialId %= 1000;
    tryday ^= serialId;
    tryday = 999 - tryday;
    return tryday;
}

void AuthService::ensureSerial()
{
    if (!m_settings)
        return;
    if (m_settings->serial.toULongLong() != 0)
        return;

    const QDateTime now = QDateTime::currentDateTime();
    const int randNum = qrand() % 999999;
    quint64 serialNum = quint64(now.date().year()) * quint64(1e13)
                        + quint64(now.date().month()) * quint64(1e11)
                        + quint64(now.date().day()) * quint64(1e9);
    serialNum += quint64(now.time().hour()) * quint64(1e7)
                 + quint64(now.time().minute()) * quint64(1e5)
                 + quint64(now.time().second()) * quint64(1e3);
    serialNum += quint64(randNum);
    serialNum %= kSerialMax;
    serialNum ^= kSerialNumKey;
    m_settings->serial = QString::number(serialNum);
    m_settings->save();
}

bool AuthService::activate(quint64 activeNum, bool permanentCheck)
{
    if (!m_settings)
        return false;

    const quint64 serialNum = m_settings->serial.toULongLong();
    bool ok = true;

    if (activeNum >= kActiveCodeTry)
    {
        const quint64 expected = generateActiveCode(serialNum, 1);
        const quint64 temp = activeNum / 1000 * 1000;
        if (expected == temp)
        {
            const quint32 tryday = tryDaysFromActiveCode(activeNum, m_settings->serialId);
            m_settings->serialId = 0;
            if (tryday == 0)
            {
                m_settings->tryDay = 0;
                m_settings->sysUsedSec = 0;
            }
            else
            {
                m_settings->tryDay += int(tryday);
            }
            m_settings->bActive = false;
        }
        else
        {
            ok = false;
        }
    }
    else if (permanentCheck)
    {
        const quint64 expected = generateActiveCode(serialNum, 0);
        if (expected == activeNum)
        {
            m_settings->bActive = true;
            m_settings->serialId = 0;
            m_settings->tryDay = 0;
            m_settings->sysUsedSec = 0;
        }
        else
        {
            ok = false;
        }
    }

    if (ok)
    {
        m_settings->license = QString::number(activeNum);
        m_settings->save();
        emit authChanged();
    }
    return ok;
}

bool AuthService::checkProbationExpired() const
{
    if (!m_settings || m_settings->bActive)
        return false;
    if (m_settings->tryDay <= 0)
        return true;
    const quint32 usedDays = m_settings->sysUsedSec / (24 * 3600);
    return usedDays >= quint32(m_settings->tryDay);
}
