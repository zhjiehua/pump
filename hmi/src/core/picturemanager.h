#ifndef PICTUREMANAGER_H
#define PICTUREMANAGER_H

#include <QPixmap>
#include <QString>

class PictureManager
{
public:
    enum Picture {
        ActiveNavigator,
        Bk,
        Btn,
        BtnFocus,
        Calibration,
        CalibrationFocus,
        Checked,
        Chinese,
        ChineseFocus,
        Clock,
        ClockFocus,
        ConnectEstablished,
        Disconnect,
        Down,
        DownArrow,
        English,
        EnglishFocus,
        FocusNavigator,
        Global,
        GlobalFocus,
        GlpInfo,
        GlpInfoFocus,
        Grid,
        GridFocus,
        Key,
        KeyFocus,
        Logo,
        Message,
        MessageFocus,
        NetConfig,
        NetConfigFocus,
        NormalNavigator,
        Permission,
        PermissionFocus,
        Setup,
        SetupFocus,
        Mcu,
        McuFocus,
        Unchecked,
        UncheckedFocus,
        Up,
        User,
        Weeping,
        PictureCount
    };

    static PictureManager &instance();

    bool loadAll();
    /** Drop cached QPixmaps while QApplication is still alive. */
    void shutdown();
    const QPixmap &pixmap(Picture id) const;
    QString url(Picture id) const;

    QString iconButtonStyle(Picture normal, Picture focus) const;
    QString pushButtonBorderImage(Picture id) const;
    QString labelBorderImage(Picture id) const;
    QString navButtonStyle(bool active) const;

private:
    PictureManager() = default;

    QPixmap m_pixmaps[PictureCount];
    QString m_urls[PictureCount];
    bool m_loaded = false;
};

#endif
