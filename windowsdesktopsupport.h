#ifndef WINDOWSDESKTOPSUPPORT_H
#define WINDOWSDESKTOPSUPPORT_H

#include "abstractdesktopsupport.h"

#include <QObject>
#include <QString>

#ifdef Q_OS_WIN
    #include <Windows.h>
    #include <shobjidl.h>
#endif

class WindowsDesktopSupport : public AbstractDesktopSupport
{
public:
    WindowsDesktopSupport();
    void             setWallpaper(const int desktop, QString fname);
};

#endif // WINDOWSDESKTOPSUPPORT_H
