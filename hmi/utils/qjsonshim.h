#ifndef QJSONSHIM_H
#define QJSONSHIM_H

#if QT_VERSION >= 0x050000

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#else

#include <QByteArray>
#include <QList>
#include <QString>
#include <QVector>

struct cJSON;

class QJsonArray;
class QJsonObject;

class QJsonValue
{
public:
    QJsonValue();
    QJsonValue(bool v);
    QJsonValue(int v);
    QJsonValue(double v);
    QJsonValue(const QString &v);
    QJsonValue(const QJsonObject &o);
    QJsonValue(const QJsonArray &a);

    bool isUndefined() const;
    QJsonObject toObject() const;
    QJsonArray toArray() const;
    double toDouble(double defaultValue = 0) const;
    int toInt(int defaultValue = 0) const;
    bool toBool(bool defaultValue = false) const;
    QString toString(const QString &defaultValue = QString()) const;

private:
    friend class QJsonDocument;
    friend class QJsonObject;
    friend class QJsonArray;
    explicit QJsonValue(cJSON *node);
    cJSON *m_node;
};

class QJsonObject
{
public:
    QJsonObject();
    QJsonObject(const QJsonObject &other);
    QJsonObject &operator=(const QJsonObject &other);
    ~QJsonObject();

    bool contains(const QString &key) const;
    QJsonValue value(const QString &key) const;
    void insert(const QString &key, const QJsonValue &value);

private:
    friend class QJsonDocument;
    friend class QJsonValue;
    friend class QJsonArray;
    explicit QJsonObject(cJSON *node, bool owned);
    cJSON *m_node;
    bool m_owned;
};

class QJsonArray
{
public:
    class const_iterator
    {
    public:
        const_iterator(cJSON *parent, int index);
        QJsonValue operator*() const;
        const_iterator &operator++();
        bool operator!=(const const_iterator &other) const;
    private:
        cJSON *m_parent;
        int m_index;
    };

    QJsonArray();
    QJsonArray(const QJsonArray &other);
    QJsonArray &operator=(const QJsonArray &other);
    ~QJsonArray();

    void append(const QJsonValue &value);
    const_iterator begin() const;
    const_iterator end() const;

private:
    friend class QJsonDocument;
    friend class QJsonValue;
    friend class QJsonObject;
    explicit QJsonArray(cJSON *node, bool owned);
    cJSON *m_node;
    bool m_owned;
};

class QJsonDocument
{
public:
    enum JsonFormat { Indented, Compact };

    QJsonDocument();
    QJsonDocument(const QJsonObject &object);
    ~QJsonDocument();

    static QJsonDocument fromJson(const QByteArray &json, QString *error = 0);

    bool isObject() const;
    QJsonObject object() const;
    QByteArray toJson(JsonFormat format = Indented) const;

private:
    cJSON *m_root;
};

#endif
#endif
