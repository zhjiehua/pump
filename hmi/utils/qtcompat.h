#ifndef HMI_QTCOMPAT_H
#define HMI_QTCOMPAT_H

#include <QtGlobal>

#if QT_VERSION < 0x050000
#  ifndef QStringLiteral
#    define QStringLiteral(str) QString::fromUtf8(str)
#  endif
#  ifndef qInfo
#    define qInfo() qDebug()
#  endif
#endif

#endif
