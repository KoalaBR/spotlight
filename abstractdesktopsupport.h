#ifndef ABSTRACTDESKTOPSUPPORT_H
#define ABSTRACTDESKTOPSUPPORT_H

#include <QList>
#include <QString>

enum class DesktopEnvironment { DESK_KDE,       ///< KDE-Desktop
                                DESK_GNOME,     ///< Gnome-Desktop
                                DESK_WINDOWS,   ///< Windows
                                DESK_UNKNOWN }; ///< Unknown

class AbstractDesktopSupport
{
public:
    AbstractDesktopSupport();
    virtual ~AbstractDesktopSupport() {}

    virtual int              getCurrentDesktop(void);     ///< returns the number of the current desktop
    virtual QList<QString>   getDesktopNames(void);       ///< Returns the name of the virtual desktops
    virtual void             setWallpaper(int desktop, QString filename);
    bool                     isDesktop(DesktopEnvironment desk);

    static QRect                    getDesktopSize(void);        ///< Size of primary Desktop.
protected:
    DesktopEnvironment      m_desktop;
};

#endif // ABSTRACTDESKTOPSUPPORT_H
