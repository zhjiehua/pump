#ifndef EVENTLOG_H
#define EVENTLOG_H

#include <QtGlobal>
#include <QString>

namespace EventLog {

/** Optional sink for operator-facing records (RecordStore). */
typedef void (*KeySink)(const QString &category, const QString &msg);
void setKeySink(KeySink sink);

/** Captures the caller's file/line/function; use EventLog_key(category, msg). */
class Key {
public:
    Key(const char *file, int line, const char *function)
        : m_file(file), m_line(line), m_function(function)
    {
    }
    void operator()(const QString &category, const QString &msg) const;

private:
    const char *m_file;
    int m_line;
    const char *m_function;
};

const char *runStatName(int stat);
QString pcCxthCmdName(quint8 cmd);
QString pcClarityPfcName(quint8 pfc);
QString mcuCxthCmdName(quint8 cmd);
QString alarmName(int kind);

} // namespace EventLog

#define EventLog_key EventLog::Key(__FILE__, __LINE__, Q_FUNC_INFO)

#endif
