#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QSettings>
#include <QGraphicsDropShadowEffect>
#include <QStandardPaths>
#include <QMessageBox>

#include <ctime>
#include <cstdlib>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageitem.h"
#include "tableitemdelegate.h"
#include "managetags.h"
#include "windowsdesktopsupport.h"
#include "finddialog.h"
#include "dialogrename.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    registerMetatypes();
    ui->setupUi(this);
    createCacheDirs();
    m_settings = new QSettings(m_baseDir +  "download" + QDir::separator() + C_MW_IniFile, QSettings::IniFormat);
    if (!m_database.openDatabase(m_baseDir))
        printLine(tr("Error: Could not open database"));
    m_addThread = new AddImageThread(&m_database, ui->tbwOverview);
    m_addThread->start();
    m_currProv = nullptr;
    setupConnections();
    m_changeImgTimeout.start(C_MW_TimeOut);
    m_fadeTimer.setInterval(70);
    srand(static_cast<unsigned int>(time(nullptr)));
    QGraphicsDropShadowEffect *labelTextShadowEffect = new QGraphicsDropShadowEffect(this);
    labelTextShadowEffect->setColor(QColor("#BBBBBB"));
    labelTextShadowEffect->setBlurRadius(0.4);
    labelTextShadowEffect->setOffset(1, 1);
    ui->label->setGraphicsEffect(labelTextShadowEffect );
    labelTextShadowEffect = new QGraphicsDropShadowEffect(this);
    labelTextShadowEffect->setColor(QColor("#BBBBBB"));
    labelTextShadowEffect->setBlurRadius(0.4);
    labelTextShadowEffect->setOffset(1, 1);
    ui->label_2->setGraphicsEffect(labelTextShadowEffect );
    labelTextShadowEffect = new QGraphicsDropShadowEffect(this);
    labelTextShadowEffect->setColor(QColor("#BBBBBB"));
    labelTextShadowEffect->setBlurRadius(0.4);
    labelTextShadowEffect->setOffset(1, 1);
    ui->label_3->setGraphicsEffect(labelTextShadowEffect );
    initProviders();
    loadSettings();
    ui->tbwOverview->setItemDelegate(new TableItemDelegate());
    slotChangeBackgroundTimeout();
    initContextMenu();
    initKeyboardShortcuts();
#ifdef Q_OS_LINUX
    m_desktop = new LinuxDesktopProvider();
#else
    m_desktop = new WindowsDesktopSupport();
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_provSpot;
    delete m_provBing;
    delete m_provCast;
    delete m_settings;
    if (m_addThread != nullptr)
        m_addThread->doShutdown();
}

void MainWindow::registerMetatypes()
{
    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<ImageItem >("ImageItem");
    qRegisterMetaType<Filter>("Filter");
}

void MainWindow::setupConnections()
{
    // Communication with AddImageThread
    connect(this, SIGNAL(signalDoAddImage(ImageItem,bool)), m_addThread, SLOT(doAddImage(ImageItem,bool)));
    connect(this, SIGNAL(signalDoAddNextImage()),           m_addThread, SLOT(doAddNextImage()));
    connect(this, SIGNAL(signalDoClear()),                  m_addThread, SLOT(doClear()));
    connect(this, SIGNAL(signalDoInit(Filter)),             m_addThread, SLOT(doInit(Filter)));
    connect(this, SIGNAL(signalDoShowAsFlat(bool)),         m_addThread, SLOT(doShowAsFlat(bool)));
    connect(this, SIGNAL(signalDoShowTag(int)),             m_addThread, SLOT(doShowTag(int)));
    connect(this, SIGNAL(signalDoShowTopLevel()),           m_addThread, SLOT(doShowTopLevel()));
    connect(this, SIGNAL(signalDoShutdown()),               m_addThread, SLOT(doShutdown()));

    connect(ui->pbSearch,   SIGNAL(clicked()),                  this, SLOT(clickedSearch()));
    connect(&m_downloader,  SIGNAL(jsonDownloaded(QString)),    this, SLOT(slotDownloadComplete(QString)));
    connect(&m_downloader,  SIGNAL(SSL_not_supported()),		this, SLOT(slotSSLMissing()));
    connect(&m_downloader,  SIGNAL(imageDownloaded(ImageItem)), this, SLOT(slotImageDownloadComplete(ImageItem)));
    connect(&m_changeImgTimeout, SIGNAL(timeout()),             this, SLOT(slotChangeBackgroundTimeout()));
    connect(&m_fadeTimer,   SIGNAL(timeout()),                  this, SLOT(slotFadeTimeout()));
    connect(ui->pbHide,     SIGNAL(clicked()),                  this, SLOT(clickedHideGUI()));
    connect(ui->pbBack,     SIGNAL(clicked()),                  this, SLOT(clickedShowGUI()));
    connect(ui->cmbDisplay, SIGNAL(currentIndexChanged(int)),   this, SLOT(slotDisplayChanged(int)));
    connect(ui->tbwOverview,SIGNAL(cellDoubleClicked(int,int)), this, SLOT(slotCellDoubleClicked(int,int)));
    connect(ui->tbTags,     SIGNAL(clicked(bool)),              this, SLOT(slotManageTags()));
    connect(ui->pbOpenFolder,SIGNAL(clicked()),                 this, SLOT(slotOpenFolder()));
    connect(m_addThread,    SIGNAL(signalAllImagesAdded()),     this, SLOT(slotAllImagesAdded()));
    connect(m_addThread,    SIGNAL(signalAddImage(QTableWidgetItem*, int,int, int)),
                this, SLOT(slotAddImage(QTableWidgetItem*, int,int,int)));
    connect(ui->tbwOverview, SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(slotContextMenuRequested(QPoint)));
    connect(ui->cmbOrientation, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOrientationChanged(int)));
}

void MainWindow::setWindowSize(const QImage &img)
{
    int width  = img.width();
    int height = img.height();
    QRect size = AbstractDesktopSupport::getDesktopSize();
    if ((width >  size.width()) || (height > size.height()))
    {
        width  = size.width();
        height = size.height();
    }
    this->setFixedSize(width, height);

}

void MainWindow::showBackgroundImage(QString fname)
{
    m_changeImgTimeout.stop();
    m_title = "";
    if (m_imgNew.load(fname))
    {
        setWindowSize(m_imgNew);
        this->setFixedSize(m_imgNew.width(), m_imgNew.height());
        this->centralWidget()->repaint();
        clickedHideGUI();
    }
    m_changeImgTimeout.start(C_MW_TimeOut);

}

void MainWindow::initContextMenu(void)
{
    m_contextMenu = new QMenu(this);
    m_contextMenu->addAction(new QAction(tr("Set as Background")));
    m_contextMenu->addAction(new QAction(tr("Delete")));
    m_contextMenu->addAction(new QAction(tr("reload")));
    m_tags = new QMenu(tr("Tag Image"));
    m_contextMenu->addMenu(m_tags);
    addTags(m_tags);
    m_contextMenu->addAction(new QAction(tr("Show")));
    m_contextMenu->addAction(new QAction(tr("Set title")));
}

void MainWindow::clickedHideGUI(void)
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::clickedShowGUI()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::slotManageTags()
{
    ManageTags tags(&m_database, this);
    tags.exec();
    delete m_tags;
    // update the context menu with the new tags
    initContextMenu();
    // reset display with new tags
    slotDisplayChanged(ui->cmbDisplay->currentIndex());
}

/**
 * @brief MainWindow::clickedSearch
 * Start a new search for images
 */
void MainWindow::clickedSearch(void)
{
    // make sure that chromecast respects what we are searching
    // 0 = Portrait, 1 = landscape, 2 = both
    m_addThread->doClear();
    if (ui->cmbDisplay->currentIndex() > 0)        // By Tags
    {
        // Switch to Tag "New"
        ui->tbwOverview->clear();
        ui->tbwOverview->setColumnCount(4);
        ui->tbwOverview->setRowCount(0);
        m_addThread->doShowTag(1);
    }
    m_provCast->setFormat(static_cast<Format>(ui->cmbOrientation->currentIndex()));
    QUrl url = QUrl(createFirstRequest());
    ui->teShowActions->clear();
    m_downloader.downloadJSON(url);
}

void MainWindow::slotChangeBackgroundTimeout(void)
{
    m_imgOld = m_imgNew;
    ImageItem item = m_database.getRandomImage();
    if (item.image().isNull())
        return;
    m_title = "";
    m_fade  = 0.0;
    if (m_imgNew.load(m_baseDir + item.filename()))
    {
        m_changeImgTimeout.stop();
        m_fadeTimer.start();
        m_title = item.title();
        setWindowSize(m_imgNew);
        this->centralWidget()->repaint();
    }
}

void MainWindow::slotFadeTimeout()
{
    this->centralWidget()->repaint();
    m_fade += 0.10;
    if (m_fade > 1.0)
    {
        m_fadeTimer.stop();
        m_changeImgTimeout.start();
    }
}

void MainWindow::slotFindImage()
{
    FindDialog find(&m_database, this);
    connect(&find, SIGNAL(signalShowImage(ImageItem)), this, SLOT(slotShowImage(ImageItem)));
    find.exec();
    disconnect(&find, SIGNAL(signalShowImage(ImageItem)), this, SLOT(slotShowImage(ImageItem)));
}


/**
 * @brief MainWindow::slotDownloadComplete
 * Will be called once a none image file has been downloaded (probably JSON or
 * HTML)
 * @param content
 */
void MainWindow::slotDownloadComplete(QString content)
{
    printLine(tr("Daten geladen - Ermittle URLs"));
    QList<ImageItem> itemList = getItemList(content.toUtf8());
    for (int i = 0; i < itemList.size(); i++)
    {
        int orientation = ui->cmbOrientation->currentIndex();
        ImageItem item = itemList[i];
        if (item.isPortrait() && (orientation != 1))    // 0 = Portrait, 2 = both
        {
            // download data
            if (m_database.canDownloadImage(item))
            {
                m_downloader.downloadImage(item);
                printLine(tr("Downloading portrait image"));
            }
        }
        else
        if ((!item.isPortrait()) && (orientation != 0)) // 1 = landscape, 2 = both
        {
            // download

            if (m_database.canDownloadImage(item))
            {
                printLine(tr("Download landscape image"));
                m_downloader.downloadImage(item);
            }
        }
    }
    printLine(tr("Done"));
}

void MainWindow::printLine(QString line)
{
    ui->teShowActions->moveCursor(QTextCursor::Start);
    ui->teShowActions->insertPlainText(line+"\r");
}

void MainWindow::saveSettings(void)
{
    m_settings->setValue("orientation", ui->cmbOrientation->currentIndex());
    m_settings->setValue("title",  ui->cmbTitle->currentIndex());
    m_settings->setValue("geometry", this->geometry());
    m_settings->setValue("display", ui->cmbDisplay->currentIndex());
}

void MainWindow::loadSettings(void)
{
//    QSettings   settings(m_baseDir + "download" + QDir::separator() + C_MW_IniFile, QSettings::IniFormat);
    ui->cmbOrientation->setCurrentIndex(m_settings->value("orientation", 0).toInt());
    ui->cmbTitle->setCurrentIndex(m_settings->value("title", 0).toInt());
    QRect rect = m_settings->value("geometry").toRect();
    if (rect.isValid())
        this->setGeometry(rect);
    ui->cmbDisplay->setCurrentIndex(m_settings->value("display", 0).toInt());
    slotOrientationChanged(m_settings->value("orientation", 0).toInt());
 }

QList<ImageItem> MainWindow::getItemList(QByteArray data)
{
    return m_currProv->getItemList(data);
}

QString MainWindow::createFirstRequest(void)
{
    if (m_provBing->canCreateNextRequest())
        m_currProv = m_provBing;
    else
    {
        m_currProv = m_provSpot;
        int index = rand() % 6;
        if (index < 3)
            m_currProv = m_provCast;
        else m_currProv = m_provSpot;
    }
    return m_currProv->createFirstRequest();
}

void MainWindow::createCacheDirs(void)
{
    m_baseDir = "";
    if (QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).size() > 0)
        m_baseDir = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).at(0);
    if (!m_baseDir.endsWith(QDir::separator()))
        m_baseDir += QDir::separator();
    QString replace = QString("") + QDir::separator();
	m_baseDir = m_baseDir.replace("/", replace);
    QString path = m_baseDir + QString("download") + QDir::separator() + "spotlight";
    QDir md = QDir();
    md.mkpath(path + QDir::separator() + "portrait");
    md.mkpath(path + QDir::separator() + "landscape");
    path = m_baseDir + QString("download") + QDir::separator() + "bing";
    md.mkpath(path + QDir::separator() + "landscape");
    md.mkpath(path + QDir::separator() + "portrait");
    path = m_baseDir + QString("download") + QDir::separator() + "chromecast";
    md.mkpath(path + QDir::separator() + "landscape");
    md.mkpath(path + QDir::separator() + "portrait");
}

void MainWindow::slotImageDownloadComplete(ImageItem item)
{
    QString filename = m_baseDir + item.filename();
    item.image().save(filename);
    m_database.addImage(item);
    printLine(tr("Runter geladen:") + item.title());
    m_addThread->doAddImage(item, true);
}

void MainWindow::slotAllImagesAdded()
{
    ui->tbwOverview->scrollToBottom();
}

void MainWindow::slotAddImage(QTableWidgetItem *item, int row, int col, int height)
{
//    qDebug() << "Index=" << item->data(C_AIT_Index).toInt() << item->toolTip();
    if (row+1 >= ui->tbwOverview->rowCount())
        ui->tbwOverview->setRowCount(row+1);
    ui->tbwOverview->setItem(row, col, item);
    ui->tbwOverview->setRowHeight(row, height+8);
    ui->tbwOverview->setColumnWidth(col, 160);
    m_addThread->doAddNextImage();
}

/*
    Create absolute path to image. This is needed to set the wallpaper
    in KDE.
 */
QString MainWindow::createStoredImageFilename(ImageItem &img)
{
    QString fname = m_baseDir + img.filename();
    return fname;
}

void MainWindow::slotContextMenuRequested(QPoint pos)
{
    QTableWidgetItem *item = ui->tbwOverview->currentItem();
    if (item == nullptr)
        return;
    int index = item->data(Qt::UserRole +1).toInt();
    ImageItem img = m_addThread->getItem(index);
    if (img.image().isNull())
        return;
    markTagSelection(m_tags, img);
    QAction *action = m_contextMenu->exec(ui->tbwOverview->viewport()->mapToGlobal(pos));
    if (action == nullptr)
        return;
    QString fname = createStoredImageFilename(img);
    if (action->text() == tr("Set as Background"))
    {
        m_desktop->setWallpaper(-1, fname);
    }
    else
    if (action->text() == tr("Show"))
    {
        showBackgroundImage(fname);
        m_title = img.title();
    }
    else
    if (action->text() == tr("Delete"))
    {
        QFile file(fname);
        bool res = file.remove();
        if (res)
        {
            item->setBackground(Qt::red);
            m_database.deleteImage(img);
        }
        else printLine(tr("Konnte Bild nicht löschen"));
    }
    else
    if (action->text() == tr("Set title"))
    {
        renameImage(img);
    }
}

void MainWindow::slotTagged(void)
{
    auto *act    = dynamic_cast<QAction*>(sender());
    int  tagid   = act->data().toInt();
    bool checked = act->isChecked();
    QTableWidgetItem *item = ui->tbwOverview->currentItem();
    if (item == nullptr)
        return;
    int index = item->data(Qt::UserRole +1).toInt();
    ImageItem img = m_addThread->getItem(index);
    m_database.tagImage(checked, tagid, img.id());
}

void MainWindow::slotDisplayChanged(int index)
{
    if (index == 0)
        m_addThread->doShowAsFlat(true);
    else m_addThread->doShowAsFlat(false);
    ui->tbwOverview->setRowCount(0);
    ui->tbwOverview->clearContents();
    m_addThread->doClear();
    m_addThread->doInit(getCurrentFilter());
}

Filter MainWindow::getCurrentFilter(void)
{
    int index = ui->cmbOrientation->currentIndex();
    Filter fi = Filter::FI_ALL;
    if (index == 0)  // Portrait
        fi = Filter::FI_PORTRAIT;
    else
    if (index == 1)  // Landscape)
        fi = Filter::FI_LANDSCAPE;
    return fi;
}

void MainWindow::slotOpenFolder()
{
    AbstractDesktopSupport::openFolder(m_baseDir + "download");
}

void MainWindow::slotOrientationChanged(int index)
{
    Q_UNUSED(index);
    Filter fi = getCurrentFilter();
    m_database.setFilter(fi);
    ui->tbwOverview->clearContents();
    m_addThread->doInit(fi);
    slotChangeBackgroundTimeout();      // Change background
}

void MainWindow::slotShowImage(ImageItem img)
{
    QString fname = createStoredImageFilename(img);
    showBackgroundImage(fname);
    m_title = img.title();
    clickedHideGUI();
}

void MainWindow::slotSSLMissing()
{
    QString msg = tr("Ohne SSL Funktionalität lassen sich keine Bilder laden.");
    msg += "\n" + tr("Bitte stellen Sie sicher, dass sich diese DLLs im ");
    msg += tr("Programmverzeichnis befinden:") + "\n";
    msg += " * libeay32.dll\n";
    msg += " * ssleay32.dll\n";
    QMessageBox::critical(this, tr("SSL Unterstützung fehlt"), msg, QMessageBox::Ok);
}

void MainWindow::slotCellDoubleClicked(int row, int col)
{
    if (ui->cmbDisplay->currentIndex() == 0)    // Flat display
        return;     // Nothing to do
    QTableWidgetItem *item = ui->tbwOverview->item(row, col);
    if (item == nullptr)
        return;
    int id = item->data(Qt::UserRole +3).toInt();
    DisplayCommand cmd = static_cast<DisplayCommand>(item->data(Qt::UserRole + 2).toInt());
    if ((id == 0) && (cmd == DisplayCommand::DIS_PIC))
       return;
    ui->tbwOverview->clear();
    ui->tbwOverview->setColumnCount(4);
    ui->tbwOverview->setRowCount(0);
    if (cmd == DisplayCommand::DIS_TAG)
        m_addThread->doShowTag(id);
    if (cmd == DisplayCommand::DIS_UP)
        m_addThread->doShowTopLevel();
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    QSize size = this->size();
    if (!m_imgOld.isNull())
    {
        paint.setOpacity(1.0);
        paint.drawImage(QPoint(0,0), m_imgOld.scaledToWidth(size.width(), Qt::SmoothTransformation));
    }
    if (!m_imgNew.isNull())
    {
        paint.setOpacity(m_fade);
        paint.drawImage(QPoint(0,0), m_imgNew.scaledToWidth(size.width(), Qt::SmoothTransformation));
        if ((ui->cmbTitle->currentIndex() == 1) && (m_title.length() > 0))
        {
            QPainterPath path;
            QFont font(paint.font());
            font.setPixelSize(30);
            int width = QFontMetrics(font).width(m_title);
            width = (this->width() - width) >>1;
            path.addText(width,40,font, m_title);
            QPen pen(Qt::black);
            pen.setWidth(6);
            paint.strokePath(path, pen);
            paint.fillPath(path, QBrush(Qt::white));
        }
    }
    QMainWindow::paintEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    m_addThread->doInit(getCurrentFilter());

}

/**
 * @brief MainWindow::renameImage
 * Called from the context menu. Allows to set a new title for
 * a given image
 * @param item the image to be renamed
 */
void MainWindow::renameImage(ImageItem item)
{
    DialogRename rename(item, &m_database, m_baseDir, this);
    if (rename.exec() == QDialog::Accepted)
    {
        // Ok, update the overview
        ui->tbwOverview->setRowCount(0);
        ui->tbwOverview->clearContents();
        m_addThread->doClear();
        m_addThread->doInit(getCurrentFilter());
    }
}

void MainWindow::addTags(QMenu *tags)
{
    QList<Tag> list = m_database.getTags();
    for (int i = 0; i < list.size(); i++)
    {
        QAction *tag = new QAction(list[i].tag);
        tag->setCheckable(true);
        tag->setData(list[i].id);
        tags->addAction(tag);
        this->connect(tag, SIGNAL(triggered()), this, SLOT(slotTagged()));
    }
}

void MainWindow::markTagSelection(QMenu *menu, const ImageItem &item)
{
    QList<QAction*> list = menu->actions();
    for (int i = 0; i < list.size(); i++)
    {
        QAction *tag = list[i];
        int tagid = tag->data().toInt();
        tag->setChecked(m_database.isTagUsed(tagid, item.id()));
    }
}

void MainWindow::initProviders(void)
{
    m_provSpot = new SpotlightProvider(ui->teShowActions);
    m_provBing = new BingProvider(ui->teShowActions);
    m_provCast = new ChromecastProvider(ui->teShowActions);
}


void MainWindow::initKeyboardShortcuts()
{
    auto *action = new QAction(this);
    this->addAction(action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    connect(action, SIGNAL(triggered()), this, SLOT(slotFindImage()));
    action = new QAction(this);
    this->addAction(action);
    action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_B));
    connect(action, SIGNAL(triggered()), this, SLOT(clickedHideGUI()));
    action = new QAction(this);
    this->addAction(action);
    action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S));
    connect(action, SIGNAL(triggered()), this, SLOT(clickedSearch()));
    action = new QAction(this);
    this->addAction(action);
    action->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(action, SIGNAL(triggered()), this, SLOT(clickedShowGUI()));
    action = new QAction(this);
    this->addAction(action);
    action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_D));
    connect(action, SIGNAL(triggered()), this, SLOT(slotOpenFolder()));
}

