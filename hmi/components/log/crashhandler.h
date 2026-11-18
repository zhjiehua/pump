#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

/** Linux crash / SIGUSR backtrace + coredump (desktop and embedded). */

namespace CrashHandler {

/** Raise RLIMIT_CORE (like `ulimit -c unlimited`). On embedded, also try to
 *  point core_pattern at coreDir (needs root; best-effort). */
bool enableCoreDump(const char *coreDir);

void install(const char *appPath, const char *backtracePath);

} // namespace CrashHandler

#endif
