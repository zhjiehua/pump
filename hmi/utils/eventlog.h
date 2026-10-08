#ifndef EVENTLOG_H
#define EVENTLOG_H

#include <QtGlobal>
#include <QString>

namespace EventLog {

/** Optional sink for operator-facing records (RecordStore). */
typedef void (*KeySink)(const QString &category, const QString &msg);
void setKeySink(KeySink sink);

/** Write a key operational event to the rotating log (via qInfo → spdlog). */
void key(const QString &category, const QString &msg);

const char *runStatName(int stat);
QString pcCxthCmdName(quint8 cmd);
QString pcClarityPfcName(quint8 pfc);
QString mcuCxthCmdName(quint8 cmd);
QString alarmName(int kind);

} // namespace EventLog

#endif
