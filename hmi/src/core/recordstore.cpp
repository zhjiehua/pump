#include "core/recordstore.h"

#include "dualbackup/jsondualbackup.h"
#include "utils/configpaths.h"
#include "utils/eventlog.h"
#include "utils/qjsonshim.h"

#include <QDateTime>
#include <QDir>
#include <QMutexLocker>

namespace {

const int kMaxRecords = 200;
RecordStore *g_sinkStore = nullptr;

void recordSink(const QString &category, const QString &message)
{
    if (g_sinkStore)
        g_sinkStore->ingest(category, message);
}

const char *kindKey(RecordStore::Kind kind)
{
    switch (kind)
    {
    case RecordStore::Event:
        return "event";
    case RecordStore::Alarm:
        return "alarm";
    case RecordStore::Maintenance:
        return "maint";
    default:
        return "event";
    }
}

QJsonArray entriesToArray(const QVector<RecordStore::Entry> &list)
{
    QJsonArray arr;
    for (int i = 0; i < list.size(); ++i)
    {
        QJsonObject o;
        o.insert(QStringLiteral("t"), list.at(i).time);
        o.insert(QStringLiteral("c"), list.at(i).category);
        o.insert(QStringLiteral("m"), list.at(i).message);
        arr.append(o);
    }
    return arr;
}

} // namespace

RecordStore::RecordStore()
{
}

RecordStore::~RecordStore()
{
    if (g_sinkStore == this)
    {
        g_sinkStore = nullptr;
        EventLog::setKeySink(nullptr);
    }
}

void RecordStore::bindToEventLog()
{
    g_sinkStore = this;
    EventLog::setKeySink(recordSink);
    load();
}

RecordStore::Kind RecordStore::kindForCategory(const QString &category)
{
    if (category == QLatin1String("ALARM"))
        return Alarm;
    if (category == QLatin1String("MAINT"))
        return Maintenance;
    return Event;
}

void RecordStore::ingest(const QString &category, const QString &message)
{
    if (category == QLatin1String("MCU-TX")
        || category == QLatin1String("MCU-RX")
        || category == QLatin1String("PC-RX"))
        return;
    append(kindForCategory(category), category, message);
}

void RecordStore::append(Kind kind, const QString &category, const QString &message)
{
    if (kind < 0 || kind >= KindCount)
        return;

    Entry e;
    e.time = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    e.category = category;
    e.message = message;

    {
        QMutexLocker lock(&m_mutex);
        m_records[kind].prepend(e);
        while (m_records[kind].size() > kMaxRecords)
            m_records[kind].remove(m_records[kind].size() - 1);
    }
    saveKind(kind);
}

QVector<RecordStore::Entry> RecordStore::records(Kind kind) const
{
    if (kind < 0 || kind >= KindCount)
        return QVector<Entry>();
    QMutexLocker lock(&m_mutex);
    return m_records[kind];
}

void RecordStore::clear(Kind kind)
{
    if (kind < 0 || kind >= KindCount)
        return;
    {
        QMutexLocker lock(&m_mutex);
        m_records[kind].clear();
    }
    saveKind(kind);
}

QString RecordStore::filePath(Kind kind) const
{
    return QDir(ConfigPaths::recordsDir())
        .filePath(QString::fromLatin1(kindKey(kind)) + QStringLiteral(".json"));
}

QString RecordStore::backupPath(Kind kind) const
{
    return filePath(kind) + QStringLiteral(".bak");
}

void RecordStore::parseKind(Kind kind, const QJsonArray &arr)
{
    QVector<Entry> out;
    for (const QJsonValue &v : arr)
    {
        const QJsonObject o = v.toObject();
        Entry e;
        e.time = o.value(QStringLiteral("t")).toString();
        e.category = o.value(QStringLiteral("c")).toString();
        e.message = o.value(QStringLiteral("m")).toString();
        if (e.time.isEmpty() && e.message.isEmpty())
            continue;
        out.append(e);
        if (out.size() >= kMaxRecords)
            break;
    }
    m_records[kind] = out;
}

bool RecordStore::loadKindUnlocked(Kind kind)
{
    const auto result = JsonDualBackup::load(filePath(kind), backupPath(kind));
    if (result.source == JsonDualBackup::Source::None)
        return false;

    const QJsonDocument doc = QJsonDocument::fromJson(result.data);
    if (!doc.isArray())
        return false;
    parseKind(kind, doc.array());
    return true;
}

void RecordStore::load()
{
    QMutexLocker lock(&m_mutex);
    for (int k = 0; k < KindCount; ++k)
        loadKindUnlocked(Kind(k));
}

QByteArray RecordStore::serializeKind(Kind kind) const
{
    return QJsonDocument(entriesToArray(m_records[kind])).toJson(QJsonDocument::Indented);
}

QByteArray RecordStore::serialize() const
{
    QJsonObject root;
    for (int k = 0; k < KindCount; ++k)
        root.insert(QString::fromLatin1(kindKey(Kind(k))), entriesToArray(m_records[k]));
    return QJsonDocument(root).toJson(QJsonDocument::Indented);
}

bool RecordStore::saveKind(Kind kind) const
{
    if (kind < 0 || kind >= KindCount)
        return false;
    QMutexLocker lock(&m_mutex);
    return JsonDualBackup::save(filePath(kind), backupPath(kind), serializeKind(kind));
}

bool RecordStore::save() const
{
    bool ok = true;
    for (int k = 0; k < KindCount; ++k)
        ok = saveKind(Kind(k)) && ok;
    return ok;
}

QByteArray RecordStore::exportJson() const
{
    QMutexLocker lock(&m_mutex);
    return serialize();
}
