#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif

#include "log/crashhandler.h"

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

namespace {

char g_appPath[512];
char g_btPath[512];

void writeRaw(int fd, const char *s)
{
    if (!s)
        return;
    size_t n = 0;
    while (s[n])
        ++n;
    if (n)
        ::write(fd, s, n);
}

void resolveFrame(FILE *fp, int index, void *addr)
{
    Dl_info dli;
    memset(&dli, 0, sizeof(dli));
    if (!dladdr(addr, &dli) || !dli.dli_fname || !dli.dli_fname[0])
    {
        fprintf(fp, "#%-2d %p\n", index, addr);
        return;
    }

    const ptrdiff_t relative = static_cast<char *>(addr)
                               - static_cast<char *>(dli.dli_fbase);

    fprintf(fp, "#%-2d %p %s(+0x%tx)", index, addr, dli.dli_fname, relative);

    if (dli.dli_sname && dli.dli_sname[0])
    {
        int status = 0;
        char *demangled = abi::__cxa_demangle(dli.dli_sname, 0, 0, &status);
        fprintf(fp, " %s", (status == 0 && demangled) ? demangled : dli.dli_sname);
        free(demangled);
        if (dli.dli_saddr)
        {
            const ptrdiff_t so = static_cast<char *>(addr)
                                 - static_cast<char *>(dli.dli_saddr);
            fprintf(fp, "+0x%tx", so);
        }
    }
    fprintf(fp, "\n");
}

void dumpStack(FILE *fp, int sig)
{
    if (!fp)
        return;

    fprintf(fp, "\nBEGIN=========================================================\n");

    time_t now = time(0);
    struct tm tmv;
    memset(&tmv, 0, sizeof(tmv));
    localtime_r(&now, &tmv);
    char when[64];
    strftime(when, sizeof(when), "%Y-%m-%d %H:%M:%S", &tmv);

    fprintf(fp, "Software(\"%s\") Crash At: %s\n", g_appPath, when);
    fprintf(fp, "signal is -> %s (%d)\n\n", strsignal(sig), sig);

    void *bt[64];
    const int n = backtrace(bt, static_cast<int>(sizeof(bt) / sizeof(bt[0])));

    fprintf(fp, "Addresses:\n");
    for (int i = 0; i < n; ++i)
        fprintf(fp, "  #%d %p\n", i, bt[i]);

    fprintf(fp, "\nFrames (module+offset for host addr2line -ifC -e <bin> 0xOFF):\n");
    for (int i = 0; i < n; ++i)
        resolveFrame(fp, i, bt[i]);

    fprintf(fp, "=========================================================END\n");
    fflush(fp);
}

void dumpToFileAndStderr(int sig)
{
    dumpStack(stderr, sig);

    if (!g_btPath[0])
        return;

    FILE *fp = fopen(g_btPath, "a");
    if (!fp)
        return;
    dumpStack(fp, sig);
    fclose(fp);
}

void crashHandler(int sig)
{
    writeRaw(STDERR_FILENO, "\n*** crash backtrace ***\n");
    dumpToFileAndStderr(sig);
    signal(sig, SIG_DFL);
    raise(sig);
}

void debugHandler(int sig)
{
    writeRaw(STDERR_FILENO, "\n*** debug backtrace ***\n");
    dumpToFileAndStderr(sig);
}

void catchSignal(int sig, void (*handler)(int), int flags)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = flags;
    sigaction(sig, &sa, 0);
}

} // namespace

void CrashHandler::install(const char *appPath, const char *backtracePath)
{
    snprintf(g_appPath, sizeof(g_appPath), "%s", appPath ? appPath : "");
    snprintf(g_btPath, sizeof(g_btPath), "%s", backtracePath ? backtracePath : "");

    catchSignal(SIGUSR1, debugHandler, SA_RESTART);
    catchSignal(SIGUSR2, debugHandler, SA_RESTART);

    const int fatalFlags = SA_RESETHAND | SA_RESTART;
    catchSignal(SIGILL, crashHandler, fatalFlags);
    catchSignal(SIGBUS, crashHandler, fatalFlags);
    catchSignal(SIGFPE, crashHandler, fatalFlags);
    catchSignal(SIGSEGV, crashHandler, fatalFlags);
    catchSignal(SIGABRT, crashHandler, fatalFlags);
}
