#include "core/picturemanager.h"

#include <QDebug>

namespace {

const char *pictureFile(PictureManager::Picture id)
{
    switch (id)
    {
    case PictureManager::ActiveNavigator: return "activenavigator.png";
    case PictureManager::Bk: return "bk.png";
    case PictureManager::Btn: return "btn.png";
    case PictureManager::BtnFocus: return "btnfocus.png";
    case PictureManager::Calibration: return "calibration.png";
    case PictureManager::CalibrationFocus: return "calibrationfocus.png";
    case PictureManager::Checked: return "checked.png";
    case PictureManager::Chinese: return "chinese.png";
    case PictureManager::ChineseFocus: return "chinesefocus.png";
    case PictureManager::Clock: return "clock.png";
    case PictureManager::ClockFocus: return "clockfocus.png";
    case PictureManager::ConnectEstablished: return "connect_established.png";
    case PictureManager::Disconnect: return "disconnect.png";
    case PictureManager::Down: return "down.png";
    case PictureManager::DownArrow: return "down-arrow.png";
    case PictureManager::English: return "english.png";
    case PictureManager::EnglishFocus: return "englishfocus.png";
    case PictureManager::FocusNavigator: return "focusnavigator.png";
    case PictureManager::Global: return "global.png";
    case PictureManager::GlobalFocus: return "globalfocus.png";
    case PictureManager::GlpInfo: return "glpinfo.png";
    case PictureManager::GlpInfoFocus: return "glpinfofocus.png";
    case PictureManager::Grid: return "grid.png";
    case PictureManager::GridFocus: return "gridfocus.png";
    case PictureManager::Key: return "key.png";
    case PictureManager::KeyFocus: return "keyfocus.png";
    case PictureManager::Logo: return "logo.png";
    case PictureManager::Message: return "message.png";
    case PictureManager::MessageFocus: return "messagefocus.png";
    case PictureManager::NetConfig: return "netconfig.png";
    case PictureManager::NetConfigFocus: return "netconfigfocus.png";
    case PictureManager::NormalNavigator: return "normalnavigator.png";
    case PictureManager::Permission: return "permission.png";
    case PictureManager::PermissionFocus: return "permissionfocus.png";
    case PictureManager::Setup: return "setup.png";
    case PictureManager::SetupFocus: return "setupfocus.png";
    case PictureManager::Mcu: return "mcu.png";
    case PictureManager::McuFocus: return "mcufocus.png";
    case PictureManager::Unchecked: return "unchecked.png";
    case PictureManager::UncheckedFocus: return "uncheckedfocus.png";
    case PictureManager::Up: return "up.png";
    case PictureManager::User: return "user.png";
    case PictureManager::Weeping: return "weeping.png";
    default: return nullptr;
    }
}

} // namespace

PictureManager &PictureManager::instance()
{
    static PictureManager mgr;
    return mgr;
}

bool PictureManager::loadAll()
{
    bool ok = true;
    for (int i = 0; i < PictureCount; ++i)
    {
        const auto id = static_cast<Picture>(i);
        const char *file = pictureFile(id);
        if (!file)
            continue;

        const QString path = QStringLiteral(":/hmi/res/ui/") + QLatin1String(file);
        m_urls[i] = path;
        if (m_pixmaps[i].load(path))
            continue;

        qWarning() << "failed to load picture:" << file;
        ok = false;
    }
    m_loaded = ok;
    return ok;
}

void PictureManager::shutdown()
{
    for (int i = 0; i < PictureCount; ++i)
        m_pixmaps[i] = QPixmap();
    m_loaded = false;
}

const QPixmap &PictureManager::pixmap(Picture id) const
{
    return m_pixmaps[id];
}

QString PictureManager::url(Picture id) const
{
    return m_urls[id];
}

QString PictureManager::iconButtonStyle(Picture normal, Picture focus) const
{
    return QStringLiteral("QPushButton{border-image:url(%1);border:0;outline:0;}"
                          "QPushButton:focus{border-image:url(%2);outline:0;}")
        .arg(url(normal), url(focus));
}

QString PictureManager::pushButtonBorderImage(Picture id) const
{
    return QStringLiteral("QPushButton{border-image:url(%1);border:0;}").arg(url(id));
}

QString PictureManager::labelBorderImage(Picture id) const
{
    return QStringLiteral("QLabel{border-image:url(%1);}").arg(url(id));
}

QString PictureManager::navButtonStyle(bool active) const
{
    if (active)
    {
        return QStringLiteral(
                   "QPushButton{border-image:url(%1);border:2px solid green;outline:0;}"
                   "QPushButton:focus{border-image:url(%2);border:2px solid green;outline:0;}")
            .arg(url(ActiveNavigator), url(FocusNavigator));
    }
    return QStringLiteral(
               "QPushButton{border-image:url(%1);border:0;outline:0;}"
               "QPushButton:focus{border-image:url(%2);border:2px solid blue;outline:0;}")
        .arg(url(NormalNavigator), url(FocusNavigator));
}
