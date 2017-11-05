#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "filedownloader.h"
#include "imageitem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void        clickedSearch(void);
    void        slotDownloadComplete(QString    content);
    void        slotImageDownloadComplete(ImageItem item);
private:
    QString     createFirstRequest();
    void        printLine(QString line);        ///< Fügt eine Zeile am Anfang ein.
    QList<ImageItem> getItemList(QByteArray data);
    QList<ImageItem> decodeJsonList(QString value);
    QList<ImageItem> getImageItem(QJsonObject image);
    void             createCacheDirs();

    Ui::MainWindow      *ui;
    DownloadManager      m_downloader;
};

#endif // MAINWINDOW_H
