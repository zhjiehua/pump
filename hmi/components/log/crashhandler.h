#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

/** Linux crash / SIGUSR backtrace (desktop and embedded). */

namespace CrashHandler {

void install(const char *appPath, const char *backtracePath);

} // namespace CrashHandler

#endif
