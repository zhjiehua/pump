#if QT_VERSION < 0x050000

#include "qjsonshim.h"

extern "C" {
#include "cJSON.h"
}

namespace {

cJSON *duplicateNode(cJSON *node)
{
    return node ? cJSON_Duplicate(node, 1) : 0;
}

void setObjectItem(cJSON *object, const QString &key, cJSON *item)
{
    cJSON_DeleteItemFromObject(object, key.toUtf8().constData());
    cJSON_AddItemToObject(object, key.toUtf8().constData(), item);
}

} // namespace

QJsonValue::QJsonValue()
    : m_node(0)
{
}

QJsonValue::QJsonValue(bool v)
    : m_node(cJSON_CreateBool(v))
{
}

QJsonValue::QJsonValue(int v)
    : m_node(cJSON_CreateNumber(v))
{
}

QJsonValue::QJsonValue(double v)
    : m_node(cJSON_CreateNumber(v))
{
}

QJsonValue::QJsonValue(const QString &v)
    : m_node(cJSON_CreateString(v.toUtf8().constData()))
{
}

QJsonValue::QJsonValue(const QJsonObject &o)
    : m_node(duplicateNode(o.m_node))
{
}

QJsonValue::QJsonValue(const QJsonArray &a)
    : m_node(duplicateNode(a.m_node))
{
}

QJsonValue::QJsonValue(cJSON *node)
    : m_node(node)
{
}

bool QJsonValue::isUndefined() const
{
    return !m_node;
}

QJsonObject QJsonValue::toObject() const
{
    if (!m_node || !cJSON_IsObject(m_node))
        return QJsonObject();
    return QJsonObject(m_node, false);
}

QJsonArray QJsonValue::toArray() const
{
    if (!m_node || !cJSON_IsArray(m_node))
        return QJsonArray();
    return QJsonArray(m_node, false);
}

double QJsonValue::toDouble(double defaultValue) const
{
    if (!m_node || !cJSON_IsNumber(m_node))
        return defaultValue;
    return m_node->valuedouble;
}

int QJsonValue::toInt(int defaultValue) const
{
    if (!m_node || !cJSON_IsNumber(m_node))
        return defaultValue;
    return int(m_node->valueint);
}

bool QJsonValue::toBool(bool defaultValue) const
{
    if (!m_node)
        return defaultValue;
    if (cJSON_IsBool(m_node))
        return cJSON_IsTrue(m_node);
    if (cJSON_IsNumber(m_node))
        return m_node->valueint != 0;
    return defaultValue;
}

QString QJsonValue::toString(const QString &defaultValue) const
{
    if (!m_node || !cJSON_IsString(m_node) || !m_node->valuestring)
        return defaultValue;
    return QString::fromUtf8(m_node->valuestring);
}

QJsonObject::QJsonObject()
    : m_node(cJSON_CreateObject())
    , m_owned(true)
{
}

QJsonObject::QJsonObject(const QJsonObject &other)
    : m_node(duplicateNode(other.m_node))
    , m_owned(true)
{
}

QJsonObject::QJsonObject(cJSON *node, bool owned)
    : m_node(node)
    , m_owned(owned)
{
}

QJsonObject &QJsonObject::operator=(const QJsonObject &other)
{
    if (m_owned)
        cJSON_Delete(m_node);
    m_node = duplicateNode(other.m_node);
    m_owned = true;
    return *this;
}

QJsonObject::~QJsonObject()
{
    if (m_owned)
        cJSON_Delete(m_node);
}

bool QJsonObject::contains(const QString &key) const
{
    return m_node && cJSON_GetObjectItem(m_node, key.toUtf8().constData());
}

QJsonValue QJsonObject::value(const QString &key) const
{
    if (!m_node)
        return QJsonValue();
    return QJsonValue(cJSON_GetObjectItem(m_node, key.toUtf8().constData()));
}

void QJsonObject::insert(const QString &key, const QJsonValue &value)
{
    if (!m_node)
        return;
    cJSON *item = duplicateNode(value.m_node);
    if (!item)
        item = cJSON_CreateNull();
    setObjectItem(m_node, key, item);
}

QJsonArray::const_iterator::const_iterator(cJSON *parent, int index)
    : m_parent(parent)
    , m_index(index)
{
}

QJsonValue QJsonArray::const_iterator::operator*() const
{
    if (!m_parent || m_index < 0 || m_index >= cJSON_GetArraySize(m_parent))
        return QJsonValue();
    return QJsonValue(cJSON_GetArrayItem(m_parent, m_index));
}

QJsonArray::const_iterator &QJsonArray::const_iterator::operator++()
{
    ++m_index;
    return *this;
}

bool QJsonArray::const_iterator::operator!=(const const_iterator &other) const
{
    return m_index != other.m_index;
}

QJsonArray::QJsonArray()
    : m_node(cJSON_CreateArray())
    , m_owned(true)
{
}

QJsonArray::QJsonArray(const QJsonArray &other)
    : m_node(duplicateNode(other.m_node))
    , m_owned(true)
{
}

QJsonArray::QJsonArray(cJSON *node, bool owned)
    : m_node(node)
    , m_owned(owned)
{
}

QJsonArray &QJsonArray::operator=(const QJsonArray &other)
{
    if (m_owned)
        cJSON_Delete(m_node);
    m_node = duplicateNode(other.m_node);
    m_owned = true;
    return *this;
}

QJsonArray::~QJsonArray()
{
    if (m_owned)
        cJSON_Delete(m_node);
}

void QJsonArray::append(const QJsonValue &value)
{
    if (!m_node)
        return;
    cJSON *item = duplicateNode(value.m_node);
    if (!item)
        item = cJSON_CreateNull();
    cJSON_AddItemToArray(m_node, item);
}

QJsonArray::const_iterator QJsonArray::begin() const
{
    return const_iterator(m_node, 0);
}

QJsonArray::const_iterator QJsonArray::end() const
{
    return const_iterator(m_node, m_node ? cJSON_GetArraySize(m_node) : 0);
}

QJsonDocument::QJsonDocument()
    : m_root(0)
{
}

QJsonDocument::QJsonDocument(const QJsonObject &object)
    : m_root(duplicateNode(object.m_node))
{
}

QJsonDocument::~QJsonDocument()
{
    cJSON_Delete(m_root);
}

QJsonDocument QJsonDocument::fromJson(const QByteArray &json, QString *error)
{
    QJsonDocument doc;
    doc.m_root = cJSON_Parse(json.constData());
    if (!doc.m_root && error)
        *error = QString::fromLatin1("parse error");
    return doc;
}

bool QJsonDocument::isObject() const
{
    return m_root && cJSON_IsObject(m_root);
}

QJsonObject QJsonDocument::object() const
{
    if (!isObject())
        return QJsonObject();
    return QJsonObject(m_root, false);
}

QByteArray QJsonDocument::toJson(JsonFormat format) const
{
    if (!m_root)
        return QByteArray();
    char *printed = format == Indented ? cJSON_Print(m_root) : cJSON_PrintUnformatted(m_root);
    if (!printed)
        return QByteArray();
    const QByteArray out(printed);
    cJSON_free(printed);
    return out;
}

#endif
