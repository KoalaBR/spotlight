#include "abstractdesktopsupport.h"

#include <QDebug>
#include <QList>
#include <QString>

AbstractDesktopSupport::AbstractDesktopSupport()
{
    m_desktop = DesktopEnvironment::DESK_UNKNOWN;
}

int AbstractDesktopSupport::getCurrentDesktop()
{
    qDebug() << "getCurrentDesktop not implemented!";
    return 1;
}

QList<QString> AbstractDesktopSupport::getDesktopNames(void)
{
    QList<QString> list;
    qDebug() << "getDesktopNames() not implemented!";
    return list;
}

void AbstractDesktopSupport::setWallpaper(int desktop, QString filename)
{
    Q_UNUSED(desktop);
    Q_UNUSED(filename);
    qDebug() << "setWallpaper not implemented";
}

bool AbstractDesktopSupport::isDesktop(DesktopEnvironment desk)
{
    if (desk == m_desktop)
        return true;
    else return false;
}
