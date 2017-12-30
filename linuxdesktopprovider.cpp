#include "linuxdesktopprovider.h"

#include <QProcessEnvironment>
#include <QStringList>
#include <QDebug>

LinuxDesktopProvider::LinuxDesktopProvider()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // KDE-Session if KDE_FULL_SESSION=true
    // or XDG_CURRENT_DESKTOP=KDE
    if (env.contains("XDG_CURRENT_DESKTOP"))
    {
        QString desktop = env.value("XDG_CURRENT_DESKTOP").toUpper();
        if (desktop == "KDE")
            m_desktop = DesktopEnvironment::DESK_KDE;
    }
    else
    if (env.contains("XDG_SESSION_DESKTOP"))
    {
        QString desktop = env.value("XDG_SESSION_DESKTOP").toUpper();
        if (desktop == "KDE")
            m_desktop = DesktopEnvironment::DESK_KDE;
    }
}

QString LinuxDesktopProvider::getProperty(QString key)
{
    QStringList args;
    QProcess process;
    args << "-root" << key;
    process.setProgram("/usr/bin/xprop");
    process.setArguments(args);
    process.start(QIODevice::ReadOnly);
    process.waitForFinished(1000);
    QString result = process.readAllStandardOutput();
    process.close();
    // Only the part right of "="
    int pos = result.indexOf("=");
    if (pos < 0)
        return "";
    result = result.mid(pos+1).trimmed();
    return result;
}

void LinuxDesktopProvider::setWallpaper(int desktop, QString fname)
{
    readEnvironment();
    if (desktop < 0)
        desktop = m_currDesktop;
    if (m_desktop == DesktopEnvironment::DESK_KDE)
    {
        FILE *file = fopen("/tmp/change.sh", "w");

        if (file != NULL)
        {
            fprintf(file, "qdbus ");
            fprintf(file, "org.kde.plasmashell ");
            fprintf(file, "/PlasmaShell ");
            fprintf(file, "org.kde.PlasmaShell.evaluateScript ");
            fprintf(file, "'var allDesktops = desktops();print (allDesktops);for (i=0;i<allDesktops.length;i++)");
            fprintf(file, "{d = allDesktops[i];");
            fprintf(file, "d.wallpaperPlugin = \"org.kde.image\";");
            fprintf(file, "d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");");
            fprintf(file, "d.writeConfig(\"Image\", \"file://%s\")}'", fname.toStdString().c_str() );
            fclose(file);
            QFile file("/tmp/change.sh");
            file.setPermissions(QFileDevice::ExeOwner | QFileDevice::ReadOwner | QFileDevice::WriteOwner);
            QProcess::execute("/tmp/change.sh");
        }

    }
    if (m_desktop == DesktopEnvironment::DESK_GNOME)
    {
        // For gnome, this may work
        // gsettings set org.gnome.desktop.background picture-uri file:///absolute/path/to/picture.jpg
        // Don't have GNOME, so pure guesswork
        QStringList args;
        args << "set" << "org.gnome.desktop.background picture-uri"
             << "picture-uri" << "file://" + fname;
        QProcess::execute("/usr/bin/gsettings", args);
    }

}

void LinuxDesktopProvider::readEnvironment()
{
    /* how to find out, how many virtual desktops we have
        xprop -root $propname
        where propname may be:
             Name                    Type        e.g. output    description
            _NET_NUMBER_OF_DESKTOPS CARDINAL  = 4       number of desktops
            _NET_CURRENT_DESKTOP    CARDINAL  = 0       current desktop, starting from 0
            _NET_DESKTOP_NAMES      UTF8_STRING = "Arbeitsfläche 1", "Arbeitsfläche 2", "Arbeitsfläche 3", "Arbeitsfläche 4"  // name of each desktop
    */

    QString result = getProperty("_NET_DESKTOP_NAMES");
    m_names = result.split(", ");
    for (int i = 0; i < m_names.size(); i++)
        m_names[i] = m_names[i].replace('"', "");
    result = getProperty("_NET_NUMBER_OF_DESKTOPS");
    m_desktopCount = result.toInt();
    result = getProperty("_NET_CURRENT_DESKTOP");
    m_currDesktop = result.toInt();

}
