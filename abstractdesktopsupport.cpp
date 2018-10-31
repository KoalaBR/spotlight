#include "abstractdesktopsupport.h"

#include <QDebug>
#include <QList>
#include <QUrl>
#include <QString>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QScreen>

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

QRect AbstractDesktopSupport::getDesktopSize(void)
{
    return QGuiApplication::primaryScreen()->availableGeometry();
}

void AbstractDesktopSupport::openFolder(QString path)
{
    QUrl url("file://" + path);
    QDesktopServices::openUrl(url);
}

