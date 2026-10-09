#ifndef DOMAIN_UICAPABILITIES_H
#define DOMAIN_UICAPABILITIES_H

#include <QStringList>

namespace UiPageKey {
const char kFlowCalib[] = "hmi.calib.flow";
const char kPressCalib[] = "hmi.calib.press";
const char kLocalGradient[] = "hmi.gradient.local";
const char kCxthWordFactor[] = "hmi.machine.wordfactor";
const char kNet[] = "hmi.net";
}

/** Feature flags for page/chrome gating (application uiPageKeys, single-device). */
class UiCapabilities
{
public:
    static UiCapabilities fromMcuProtocol(int mcuProtocol);

    bool has(const char *key) const;
    QStringList keys() const { return m_keys; }

private:
    QStringList m_keys;
};

#endif
