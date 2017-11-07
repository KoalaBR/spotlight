#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTimer>

#include "filedownloader.h"
#include "imageitem.h"

namespace Ui {
class MainWindow;
}

#define   C_MW_TimeOut  10000           ///< Timeout in ms
#define   C_MW_IniFile  "settings.ini"  ///< Where to store settings

enum class Orientation { PORTRAIT = 0, LANDSCAPE = 1, BOTH = 2};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void        clickedSearch(void);
    void        slotChangeBackgroundTimeout(void);
    void        slotDownloadComplete(QString    content);
    void        slotImageDownloadComplete(ImageItem item);
protected:
    void        paintEvent(QPaintEvent *event);
private:
    QString     createFirstRequest();
    void        printLine(QString line);        ///< Fügt eine Zeile am Anfang ein.
    void        saveSettings(void);
    void        loadSettings(void);
    QList<ImageItem> getItemList(QByteArray data);
    QList<ImageItem> decodeJsonList(QString value);
    QList<ImageItem> getImageItem(QJsonObject image);
    void             createCacheDirs();

    Ui::MainWindow      *ui;
    DownloadManager      m_downloader;
    QTimer               m_changeImgTimeout;            ///< Timer, which changes background image if possible
    QImage               m_img1;
};

#endif // MAINWINDOW_H
