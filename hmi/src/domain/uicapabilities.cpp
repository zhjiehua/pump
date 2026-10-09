#include "domain/uicapabilities.h"
#include "core/appsettings.h"

UiCapabilities UiCapabilities::fromMcuProtocol(int mcuProtocol)
{
    UiCapabilities c;
    c.m_keys << QLatin1String(UiPageKey::kFlowCalib)
             << QLatin1String(UiPageKey::kPressCalib)
             << QLatin1String(UiPageKey::kNet);
    if (mcuProtocol != AppSettings::QinFine)
    {
        c.m_keys << QLatin1String(UiPageKey::kLocalGradient)
                 << QLatin1String(UiPageKey::kCxthWordFactor);
    }
    return c;
}

bool UiCapabilities::has(const char *key) const
{
    return m_keys.contains(QLatin1String(key));
}
