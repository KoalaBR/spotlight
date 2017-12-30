#include "windowsdesktopsupport.h"
#include <QDebug>
#include <QString>

WindowsDesktopSupport::WindowsDesktopSupport()
{

}

void WindowsDesktopSupport::setWallpaper(const int desktop, QString filename)
{
    Q_UNUSED(desktop);
    Q_UNUSED(filename);
#ifdef Q_OS_WIN
    QString fname = filename.replace("/", "\\");
    HRESULT hr = CoInitialize(nullptr);
    IDesktopWallpaper *pDesktopWallpaper = nullptr;
    hr = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));
    if (FAILED(hr))
    {
        qDebug() << "error";
    }
    else
    {
        pDesktopWallpaper->SetWallpaper(nullptr, fname.toStdWString().c_str());
    }
#endif
}
