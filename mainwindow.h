#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTimer>

#include "filedownloader.h"
#include "imageitem.h"
#include "database.h"
#include "addimagethread.h"

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
    void        clickedHideGUI(void);
    void        clickedSearch(void);
    void        clickedShowGUI(void);
    void        slotChangeBackgroundTimeout(void);
    void        slotDownloadComplete(QString    content);
    void        slotImageDownloadComplete(ImageItem item);
    void        slotAddImage(ImageItem item, int row, int col);
    void        slotDownloadsFinished(void);
    void        slotCustomMenuRequested(const QPoint pos);
protected:
    void        paintEvent(QPaintEvent *event);
    void        closeEvent(QCloseEvent *event);
    void        resizeEvent(QResizeEvent* event);
private:
    QString     createFirstRequest(void);
    void        printLine(QString line);        ///< Fügt eine Zeile am Anfang ein.
    void        saveSettings(void);
    void        loadSettings(void);
    QList<ImageItem> getItemList(QByteArray data);
    QList<ImageItem> decodeJsonList(QString value);
    QList<ImageItem> getImageItem(QJsonObject image);
    void             createCacheDirs(void);
    QString          createStoredImageFilename(ImageItem &img);

    Ui::MainWindow      *ui;
    DownloadManager      m_downloader;
    QTimer               m_changeImgTimeout;            ///< Timer, which changes background image if possible
    QImage               m_img1;
    Database             m_database;
    AddImageThread      *m_addThread;
};

#endif // MAINWINDOW_H
