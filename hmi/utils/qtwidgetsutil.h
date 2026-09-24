#ifndef QTWIDGETSUTIL_H
#define QTWIDGETSUTIL_H

#include <QComboBox>
#include <QString>
#include <QVariant>
#include <QVector>

inline void hmiComboSetCurrentText(QComboBox *box, const QString &text)
{
#if QT_VERSION >= 0x050000
    box->setCurrentText(text);
#else
    const int idx = box->findText(text);
    if (idx >= 0)
        box->setCurrentIndex(idx);
#endif
}

inline QVariant hmiComboCurrentData(const QComboBox *box)
{
#if QT_VERSION >= 0x050000
    return box->currentData();
#else
    return box->itemData(box->currentIndex());
#endif
}

template<typename T>
inline T hmiVectorTakeLast(QVector<T> *vec)
{
#if QT_VERSION >= 0x050000
    return vec->takeLast();
#else
    const T v = vec->last();
    vec->remove(vec->size() - 1);
    return v;
#endif
}

#endif
