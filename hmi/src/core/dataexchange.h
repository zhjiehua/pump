#ifndef DATAEXCHANGE_H
#define DATAEXCHANGE_H

#include <QString>

class AppSettings;
class RecordStore;

/** USB/SD (embedded) or app-dir/export (desktop) config & record packs. */
namespace DataExchange {

QString exportDir();
QString configPackPath();
QString recordsPackPath();

bool exportConfig(const AppSettings *settings, QString *error);
bool importConfig(AppSettings *settings, QString *error);
bool exportRecords(const RecordStore *records, QString *error);

} // namespace DataExchange

#endif
