#ifndef LINUXDESKTOPPROVIDER_H
#define LINUXDESKTOPPROVIDER_H

#include "abstractdesktopsupport.h"

class LinuxDesktopProvider : public AbstractDesktopSupport
{
public:
    LinuxDesktopProvider();
   ~LinuxDesktopProvider() {}
//    int              getCurrentDesktop(void);       ///< returns the number of the current desktop
//    QList<QString>   getDesktopNames(void);         ///< Returns the name of the virtual desktops
    void             setWallpaper(int desktop, QString filename);

private:
    void             readEnvironment(void);
    QString          getProperty(QString key);      ///< read property from root window

    QStringList      m_names;
    int              m_desktopCount;
    int              m_currDesktop;
};

#endif // LINUXDESKTOPPROVIDER_H
