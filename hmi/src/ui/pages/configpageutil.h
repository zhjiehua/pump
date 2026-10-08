#ifndef CONFIGPAGEUTIL_H
#define CONFIGPAGEUTIL_H

#include "ui/widgets/comboctrl.h"

#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>
#include <QWidget>

inline void setConfigFormRowVisible(QLabel *label, QWidget *field, bool visible)
{
    if (label)
        label->setVisible(visible);
    if (field)
        field->setVisible(visible);
}

inline QGroupBox *makeConfigGroup(QWidget *parent)
{
    auto *box = new QGroupBox(parent);
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return box;
}

void refreshSerialComboList(ComboCtrl *mcuPort, ComboCtrl *pcSerial);

#endif
