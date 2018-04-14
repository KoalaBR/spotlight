#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTimer>

#include "filedownloader.h"
#include "imageitem.h"
#include "database.h"
#include "addimagethread.h"
#include "spotlightprovider.h"
#include "bingprovider.h"
#include "chromecastprovider.h"
#include "linuxdesktopprovider.h"

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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void        clickedHideGUI(void);               ///< Hides most of the GUI just to show the image in the background
    void        clickedSearch(void);                ///< Start searching for new pictures
    void        clickedShowGUI();                   ///< show Widgets again
    void        slotManageTags(void);
    void        slotChangeBackgroundTimeout(void);
    void        slotFadeTimeout(void);
    void        slotFindImage(void);                ///< Search for images by title
    void        slotDownloadComplete(QString    content);
    void        slotImageDownloadComplete(ImageItem item);
    void        slotAllImagesAdded(void);
    void        slotAddImage(QTableWidgetItem *item, int row, int col, int height);
    void        slotContextMenuRequested(const QPoint pos);
    void        slotTagged(void);
    void        slotDisplayChanged(int index);
    void        slotOrientationChanged(int index);
    void        slotOpenFolder(void);               ///< Called if user wants to open desktop image folder
    void        slotCellDoubleClicked(int row, int col);
	void		slotSSLMissing(void);				///< called from DownloadManager if SSL is not supported
signals:
    void    signalDoAddImage(ImageItem item, bool newPic = false);
    void    signalDoAddNextImage(void);
    void    signalDoClear(void);
    void    signalDoInit(Filter f);
    void    signalDoShowTag(int id);
    void    signalDoShowTopLevel(void);
    void    signalDoShutdown(void);
    void    signalDoShowAsFlat(bool flat);          ///< if true, just show the pics as they come in, otherwise show with tags as directory


protected:
    void        paintEvent(QPaintEvent *event) override;
    void        closeEvent(QCloseEvent *event) override;
    void        resizeEvent(QResizeEvent* event) override;
private:
    void        addTags(QMenu *tags);           ///< Add all tags to the sub menu
    QString     createFirstRequest(void);
    void        initContextMenu(void);          ///< Create basic context menu for table
    void        initKeyboardShortcuts();
    void        initProviders(void);
    void        loadSettings(void);
    void        printLine(QString line);        ///< Fügt eine Zeile am Anfang ein.
    void        registerMetatypes();
    void        renameImage(ImageItem item);    ///< Rename image
    void        saveSettings(void);
    void        setupConnections();             ///< all needed connects here
    void        setWindowSize(const QImage &img);
    QList<ImageItem> getItemList(QByteArray data);
    QList<ImageItem> getImageItem(QJsonObject image);
    void             createCacheDirs(void);
    QString          createStoredImageFilename(ImageItem &img);
    void             markTagSelection(QMenu *menu, const ImageItem &item);
    Filter           getCurrentFilter(void);        ///< What type of image (portrait / landscape) to display?

    Ui::MainWindow      *ui;
    QString              m_title;
    DownloadManager      m_downloader;
    QTimer               m_changeImgTimeout;            ///< Timer, which changes background image if possible
    QTimer               m_fadeTimer;                   ///< Timer, for cross fade
    QImage               m_imgNew, m_imgOld;
    Database             m_database;
    AddImageThread      *m_addThread;
    double               m_fade;
    AbstractProvider    *m_currProv;
    SpotlightProvider   *m_provSpot;
    BingProvider        *m_provBing;
    ChromecastProvider  *m_provCast;
    QMenu               *m_contextMenu;                 ///< Contextmenu for images in overview
    QMenu               *m_tags;
    AbstractDesktopSupport  *m_desktop;
    QString              m_baseDir;                     ///< Where do we store the pictures?
};


#endif // MAINWINDOW_H
