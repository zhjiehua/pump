#include "ui/pages/configpageutil.h"
#include "platform/hmiserialport.h"
#include "utils/qtwidgetsutil.h"

void refreshSerialComboList(ComboCtrl *mcuPort, ComboCtrl *pcSerial)
{
    const QString mcuCur = mcuPort ? mcuPort->currentText() : QString();
    const QString pcCur = pcSerial ? pcSerial->currentText() : QString();
    if (mcuPort)
        mcuPort->clear();
    if (pcSerial)
        pcSerial->clear();
    foreach (const QString &p, HmiSerialPortInfo::availablePortNames())
    {
        if (mcuPort)
            mcuPort->addItem(p);
        if (pcSerial)
            pcSerial->addItem(p);
    }
    if (mcuPort && !mcuCur.isEmpty())
        hmiComboSetCurrentText(mcuPort, mcuCur);
    if (pcSerial && !pcCur.isEmpty())
        hmiComboSetCurrentText(pcSerial, pcCur);
}
