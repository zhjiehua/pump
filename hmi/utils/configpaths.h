#ifndef CONFIGPATHS_H
#define CONFIGPATHS_H

#include <QString>

namespace ConfigPaths {
QString writableAppConfigDir();
/** `{appDir}/data` — deviceinfo/system/data JSON (+ factory snapshots). */
QString dataDir();
/** `{appDir}/records` — event/alarm/maint JSON. */
QString recordsDir();
}

#endif
