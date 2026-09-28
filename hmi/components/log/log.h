#ifndef LOG_H
#define LOG_H

// After Log::init(), use Qt logging APIs (qDebug/qInfo/qWarning/qCritical).
// Messages are forwarded to the rotating log file via spdlog.

namespace Log {

void init();
void writeBootBanner();
void shutdown();

} // namespace Log

#endif
