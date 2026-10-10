#include "utils/eventlog.h"

#include <QDebug>

namespace EventLog {

static KeySink g_keySink = nullptr;

void setKeySink(KeySink sink)
{
    g_keySink = sink;
}

void Key::operator()(const QString &category, const QString &msg) const
{
#if QT_VERSION >= 0x050000
    QMessageLogger(m_file, m_line, m_function).info() << "[EVENT]" << category << msg;
#else
    qDebug() << "[EVENT]" << category << msg;
#endif
    if (g_keySink)
        g_keySink(category, msg);
}

const char *runStatName(int stat)
{
    switch (stat)
    {
    case 0:
        return "Stop";
    case 1:
        return "Pause";
    case 2:
        return "Running";
    case 3:
        return "Pump";
    case 4:
        return "Purge";
    case 5:
        return "PcCtrl";
    default:
        return "Unknown";
    }
}

QString pcCxthCmdName(quint8 cmd)
{
    switch (cmd)
    {
    case 0x00:
        return QStringLiteral("PFC_START");
    case 0x01:
        return QStringLiteral("PFC_PURGE");
    case 0x02:
        return QStringLiteral("PFC_STOP");
    case 0x03:
        return QStringLiteral("PFC_READ_PRESS");
    case 0x05:
        return QStringLiteral("PFC_HOLD");
    case 0x08:
        return QStringLiteral("PFC_SET_FLOW1");
    case 0x09:
        return QStringLiteral("PFC_SET_MAXPRESS");
    case 0x0A:
        return QStringLiteral("PFC_SET_MINPRESS");
    case 0x0B:
        return QStringLiteral("PFC_CALIB");
    case 0x0E:
        return QStringLiteral("PFC_TIME_SYNC");
    default:
        return QStringLiteral("PFC_0x%1").arg(cmd, 2, 16, QLatin1Char('0'));
    }
}

QString pcClarityPfcName(quint8 pfc)
{
    switch (pfc)
    {
    case 0x01:
        return QStringLiteral("PFCC_READ_ID");
    case 0x02:
        return QStringLiteral("PFCC_LICENSE_H");
    case 0x03:
        return QStringLiteral("PFCC_LICENSE_L");
    case 0x04:
        return QStringLiteral("PFCC_STATUS");
    case 0x10:
        return QStringLiteral("PFCC_SET_FLOW");
    case 0x11:
        return QStringLiteral("PFCC_SET_PERCENT");
    case 0x12:
        return QStringLiteral("PFCC_SYNCTIME");
    case 0x13:
        return QStringLiteral("PFCC_MAX_PRESS");
    case 0x14:
        return QStringLiteral("PFCC_MIN_PRESS");
    case 0x15:
        return QStringLiteral("PFCC_START");
    case 0x16:
        return QStringLiteral("PFCC_STOP");
    case 0x17:
        return QStringLiteral("PFCC_PRESSCLEAR");
    case 0x18:
        return QStringLiteral("PFCC_READ_PRESS");
    case 0x19:
        return QStringLiteral("PFCC_PURGE");
    case 0x1A:
        return QStringLiteral("PFCC_HOLD");
    case 0x41:
        return QStringLiteral("PFCC_EXT");
    case 0x44:
        return QStringLiteral("PFCC_PRESS_COMPEN");
    case 0x90:
        return QStringLiteral("PFCC_SEND_PRESS");
    default:
        return QStringLiteral("PFCC_0x%1").arg(pfc, 2, 16, QLatin1Char('0'));
    }
}

QString mcuCxthCmdName(quint8 cmd)
{
    switch (cmd)
    {
    case 0x01:
        return QStringLiteral("MCU_SET_PARAM");
    case 0x02:
        return QStringLiteral("MCU_MOTOR_INI");
    case 0x04:
        return QStringLiteral("MCU_WAVEADD_MOTOR");
    case 0x05:
        return QStringLiteral("MCU_WAVEDEC_MOTOR");
    case 0x09:
        return QStringLiteral("MCU_READ_PARAM");
    case 0x0A:
        return QStringLiteral("MCU_READ_VERSION");
    case 0x0B:
        return QStringLiteral("MCU_READ_AU_VAL");
    case 0x0C:
        return QStringLiteral("MCU_READ_AU_VALB");
    default:
        return QStringLiteral("MCU_0x%1").arg(cmd, 2, 16, QLatin1Char('0'));
    }
}

QString alarmName(int kind)
{
    switch (kind)
    {
    case 0:
        return QStringLiteral("NoWarn");
    case 1:
        return QStringLiteral("CommunicationErr");
    case 3:
        return QStringLiteral("OverpressErr");
    case 4:
        return QStringLiteral("Weeping");
    default:
        return QStringLiteral("Alarm_%1").arg(kind);
    }
}

} // namespace EventLog
