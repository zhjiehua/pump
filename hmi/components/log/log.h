#ifndef LOG_H
#define LOG_H

#include <QString>

// After Log::init(), use Qt logging APIs (qDebug/qInfo/qWarning/qCritical).
// Messages are forwarded to the rotating log file via spdlog, with
// [file:line][func()] context.

namespace Log {

void init();
void writeBootBanner();
void shutdown();
QString directory();
void flush();

} // namespace Log

#endif
