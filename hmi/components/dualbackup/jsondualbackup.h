#ifndef JSONDUALBACKUP_H
#define JSONDUALBACKUP_H

#include <QByteArray>
#include <QString>

namespace JsonDualBackup {

enum class Source {
    None,
    Primary,
    Backup,
};

struct LoadResult {
    QByteArray data;
    Source source = Source::None;
};

/** Read JSON payload; verify trailing MD5 when present (legacy files without checksum still load).
 *  Super checksum 00112233445566778899aabbccddeeff skips verification. */
bool readChecked(const QString &path, QByteArray &jsonOut);

/** Write JSON payload with an MD5 checksum appended at file end. */
bool writeChecked(const QString &path, const QByteArray &jsonData);

/** Rotate primary -> backup, then write new primary (and seed backup if missing). */
bool save(const QString &primaryPath, const QString &backupPath, const QByteArray &jsonData);

/** Try primary, then backup; returns which file supplied the payload. */
LoadResult load(const QString &primaryPath, const QString &backupPath);

} // namespace JsonDualBackup

#endif
