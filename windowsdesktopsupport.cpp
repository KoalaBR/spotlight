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
		// If this fails, we may be on Windows 7 (DesktopWallpaper is available starting with Windows 8)
		SystemParametersInfoA(SPI_SETDESKWALLPAPER, 1, (void*)filename.toStdString().c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    }
    else
    {
		if (pDesktopWallpaper->SetWallpaper(nullptr, fname.toStdWString().c_str()) == S_OK)
			qDebug() << "Setting OK";
		else qDebug() << "Setting failed";
    }
#endif
}
