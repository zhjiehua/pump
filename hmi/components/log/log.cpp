#include "log/log.h"
#include "utils/configpaths.h"
#include "utils/hmiconfig.h"
#include "utils/version.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QStringList>
#include <QSysInfo>
#include <QtGlobal>

#if defined(Q_OS_LINUX)
#include "log/crashhandler.h"
#endif

#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace {

const size_t kMaxFileBytes = 5 * 1024 * 1024;
const size_t kMaxFiles = 5;
std::string g_logFile;

#if QT_VERSION >= 0x050000
QtMessageHandler g_prevQtHandler = 0;

// Match application: [file:line][func()] with a truncated path and 5-digit line.
QByteArray formatLogContext(const QMessageLogContext &context)
{
    QString func;
    if (context.function && context.function[0] != '\0')
    {
        const QStringList segments = QString::fromUtf8(context.function).split(QLatin1Char(' '));
        for (int i = 0; i < segments.size(); ++i)
        {
            const QString &s = segments.at(i);
            const int paren = s.indexOf(QLatin1Char('('));
            if (paren >= 0)
            {
                func = s.left(paren) + QStringLiteral("()");
                func = func.split(QStringLiteral("::")).last();
                break;
            }
        }
    }

    const char *file = (context.file && context.file[0] != '\0') ? context.file : "unknown";
    const size_t fileLength = std::strlen(file);
    const char *fileShown = (fileLength >= 30) ? (file + (fileLength - 30)) : file;
    const QByteArray funcUtf8 = func.toUtf8();

    char buf[160];
    std::snprintf(buf, sizeof(buf), "[%.30s:%05d][%.20s]",
                  fileShown, context.line, funcUtf8.constData());
    return QByteArray(buf);
}

void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const std::string text = msg.toUtf8().constData();
    const QByteArray ctx = formatLogContext(context);

    switch (type)
    {
    case QtDebugMsg:
        spdlog::debug("{} -> {}", ctx.constData(), text);
        break;
    case QtInfoMsg:
        spdlog::info("{} -> {}", ctx.constData(), text);
        break;
    case QtWarningMsg:
        spdlog::warn("{} -> {}", ctx.constData(), text);
        break;
    case QtCriticalMsg:
        spdlog::error("{} -> {}", ctx.constData(), text);
        break;
    case QtFatalMsg:
        spdlog::critical("{} -> {}", ctx.constData(), text);
        break;
    }

    if (g_prevQtHandler)
        g_prevQtHandler(type, context, msg);
}
#else
QtMsgHandler g_prevQtHandler = 0;

void qtMessageHandler(QtMsgType type, const char *msg)
{
    const std::string text = msg ? msg : "";
    switch (type)
    {
    case QtDebugMsg:
        spdlog::debug("{}", text);
        break;
    case QtWarningMsg:
        spdlog::warn("{}", text);
        break;
    case QtCriticalMsg:
        spdlog::error("{}", text);
        break;
    case QtFatalMsg:
        spdlog::critical("{}", text);
        break;
    default:
        break;
    }

    if (g_prevQtHandler)
        g_prevQtHandler(type, msg);
}
#endif

QString resolveBaseDir()
{
    return ConfigPaths::writableAppConfigDir();
}

} // namespace

void Log::init()
{
    const QString logDirPath = QDir(resolveBaseDir()).filePath(QStringLiteral("logs"));
    QDir().mkpath(logDirPath);

    g_logFile = QDir(logDirPath)
                    .filePath(QStringLiteral("pump.log"))
                    .toUtf8()
                    .constData();

    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> sink(
        new spdlog::sinks::rotating_file_sink_mt(g_logFile, kMaxFileBytes, kMaxFiles));

    std::shared_ptr<spdlog::logger> logger(
        new spdlog::logger(QStringLiteral("hmi").toUtf8().constData(), sink));
    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::info);
    spdlog::set_default_logger(logger);
    spdlog::set_pattern(QStringLiteral("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][%t]%v").toUtf8().constData());

#if QT_VERSION >= 0x050000
    g_prevQtHandler = qInstallMessageHandler(qtMessageHandler);
#else
    g_prevQtHandler = qInstallMsgHandler(qtMessageHandler);
#endif

#if defined(Q_OS_LINUX) && !defined(__SANITIZE_ADDRESS__)
    const QByteArray appPath = QCoreApplication::applicationFilePath().toUtf8();
    const QByteArray btPath = QDir(logDirPath)
                                  .filePath(QStringLiteral("backtrace"))
                                  .toUtf8();
    CrashHandler::install(appPath.constData(), btPath.constData());
#endif
}

void Log::writeBootBanner()
{
#if HMI_EMBEDDED
    const char *platform = "Embedded Linux";
#else
    const char *platform = "Desktop";
#endif

    const std::string bootTime = QDateTime::currentDateTime()
                                     .toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
                                     .toUtf8()
                                     .constData();
    const std::string binary = QCoreApplication::applicationFilePath().toUtf8().constData();
#if QT_VERSION >= 0x050000
    const std::string kernel = QSysInfo::prettyProductName().toUtf8().constData();
    const std::string cpuArch = QSysInfo::currentCpuArchitecture().toUtf8().constData();
#else
    const std::string kernel = "Linux";
    const std::string cpuArch =
#if defined(__aarch64__)
        "arm64";
#elif defined(__arm__)
        "arm";
#elif defined(__x86_64__)
        "x86_64";
#else
        "unknown";
#endif
#endif

    SPDLOG_INFO("============================================================");
    SPDLOG_INFO("  >> Pump  HPLC PUMP HMI  |  CXTH <<");
    SPDLOG_INFO("     Chromatography Control Panel");
    SPDLOG_INFO("------------------------------------------------------------");
    SPDLOG_INFO("  Product  : Pump Chromatography Control Panel");
    SPDLOG_INFO("  Vendor   : CXTH");
    SPDLOG_INFO("  Version  : {}", HMI_APP_VERSION);
    SPDLOG_INFO("  Platform : {} | Qt {} | {}", platform, QT_VERSION_STR, cpuArch);
    SPDLOG_INFO("  System   : {}", kernel);
    SPDLOG_INFO("  Built    : {} {}", __DATE__, __TIME__);
    SPDLOG_INFO("  Boot     : {}", bootTime);
    SPDLOG_INFO("  Binary   : {}", binary);
    SPDLOG_INFO("  Log file : {}", g_logFile);
#if defined(Q_OS_LINUX) && !defined(__SANITIZE_ADDRESS__)
    const QByteArray btFile = QDir(directory())
                                  .filePath(QStringLiteral("backtrace"))
                                  .toUtf8();
    SPDLOG_INFO("  Backtrace: {}  (kill -USR1 <pid>)", btFile.constData());
#endif
    SPDLOG_INFO("============================================================");
    SPDLOG_INFO("BOOT OK — Pump HMI ready");
}

QString Log::directory()
{
    return QDir(resolveBaseDir()).filePath(QStringLiteral("logs"));
}

void Log::flush()
{
    std::shared_ptr<spdlog::logger> logger = spdlog::default_logger();
    if (logger)
        logger->flush();
}

void Log::shutdown()
{
    SPDLOG_INFO("logger shutdown");
#if QT_VERSION >= 0x050000
    qInstallMessageHandler(g_prevQtHandler);
#else
    qInstallMsgHandler(g_prevQtHandler);
#endif
    g_prevQtHandler = 0;
    spdlog::shutdown();
}
