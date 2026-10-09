#ifndef RECORDSTORE_H
#define RECORDSTORE_H

#include <QByteArray>
#include <QMutex>
#include <QString>
#include <QVector>

class RecordStore
{
public:
    enum Kind {
        Event = 0,
        Alarm,
        Maintenance,
        KindCount
    };

    struct Entry {
        QString time;
        QString category;
        QString message;
    };

    RecordStore();
    ~RecordStore();

    void bindToEventLog();
    void ingest(const QString &category, const QString &message);
    void append(Kind kind, const QString &category, const QString &message);
    QVector<Entry> records(Kind kind) const;
    void clear(Kind kind);
    void clearAll();
    void load();
    bool save() const;
    QByteArray exportJson() const;

private:
    static Kind kindForCategory(const QString &category);
    QString filePath(Kind kind) const;
    QString backupPath(Kind kind) const;
    QByteArray serialize() const;
    QByteArray serializeKind(Kind kind) const;
    bool saveKind(Kind kind) const;
    bool loadKindUnlocked(Kind kind);
    void parseKind(Kind kind, const class QJsonArray &arr);

    mutable QMutex m_mutex;
    QVector<Entry> m_records[KindCount];
};

#endif
