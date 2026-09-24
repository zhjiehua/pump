#ifndef SPATIALFOCUS_H
#define SPATIALFOCUS_H

#include <QList>

class QWidget;

namespace HmiSpatialFocus {

QList<QWidget *> collectFocusables(QWidget *root);

/** Top-left focusable widget in @a root (for first focus). */
QWidget *defaultWidget(QWidget *root);

/** Next widget in direction from @a current (may be null). */
QWidget *neighbor(QWidget *root, QWidget *current, int key);

} // namespace HmiSpatialFocus

#endif
