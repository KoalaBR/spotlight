#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QSettings>
#include <QGraphicsDropShadowEffect>
#include <QProcess>

#include <time.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageitem.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createCacheDirs();
    m_addThread = new AddImageThread(&m_database, ui->tbwOverview);
    m_addThread->start();
    m_currProv = NULL;
    connect(ui->pbSearch,   SIGNAL(clicked()),                  this, SLOT(clickedSearch()));
    connect(&m_downloader,  SIGNAL(jsonDownloaded(QString)),    this, SLOT(slotDownloadComplete(QString)));
    connect(&m_downloader,  SIGNAL(imageDownloaded(ImageItem)), this, SLOT(slotImageDownloadComplete(ImageItem)));
    connect(&m_changeImgTimeout, SIGNAL(timeout()),             this, SLOT(slotChangeBackgroundTimeout()));
    connect(&m_fadeTimer,   SIGNAL(timeout()),                  this, SLOT(slotFadeTimeout()));
    connect(ui->pbHide,     SIGNAL(clicked()),                  this, SLOT(clickedHideGUI()));
    connect(ui->pbBack,     SIGNAL(clicked()),                  this, SLOT(clickedShowGUI()));
    connect(m_addThread,    SIGNAL(signalAddImage(QTableWidgetItem*, int,int, int)),
                this, SLOT(slotAddImage(QTableWidgetItem*, int,int,int)));
    connect(ui->tbwOverview, SIGNAL(customContextMenuRequested(const QPoint)),
                this, SLOT(slotContextMenuRequested(const QPoint)));
    m_changeImgTimeout.start(C_MW_TimeOut);
    m_fadeTimer.setInterval(70);
    srand(static_cast<unsigned int>(time(NULL)));
    QGraphicsDropShadowEffect *labelTextShadowEffect = new QGraphicsDropShadowEffect(this);
    labelTextShadowEffect->setColor(QColor("#BBBBBB"));
    labelTextShadowEffect->setBlurRadius(0.4);
    labelTextShadowEffect->setOffset(1, 1);
    ui->label->setGraphicsEffect(labelTextShadowEffect );
    labelTextShadowEffect = new QGraphicsDropShadowEffect(this);
    labelTextShadowEffect->setColor(QColor("#BBBBBB"));
    labelTextShadowEffect->setBlurRadius(0.4);
    labelTextShadowEffect->setOffset(1, 1);
    ui->label_3->setGraphicsEffect(labelTextShadowEffect );
    initProviders();
    loadSettings();
    if (!m_database.openDatabase())
        printLine(tr("Error: Could not open database"));
    slotChangeBackgroundTimeout();
    QList<Tag> list = m_database.getTags();
    initContextMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_provSpot;
    delete m_provBing;
    delete m_provCast;
    if (m_addThread != NULL)
        m_addThread->doShutdown();
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
}

void MainWindow::clickedHideGUI(void)
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->pgbProgress->hide();
    ui->lblText->hide();
}

void MainWindow::clickedShowGUI(void)
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::clickedSearch(void)
{
    // make sure that chromecast respects what we are searching
    // 0 = Portrait, 1 = landscape, 2 = both
    m_provCast->setFormat(static_cast<Format>(ui->cmbOrientation->currentIndex()));
    QUrl url = QUrl(createFirstRequest());
    ui->teShowActions->clear();
    m_addThread->doClear();
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
    if (m_imgNew.load(item.filename()))
    {
        m_changeImgTimeout.stop();
        m_fadeTimer.start();
        m_title = item.title();
        this->setFixedSize(m_imgNew.width(), m_imgNew.height());
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


/**
 * @brief MainWindow::slotDownloadComplete
 * Will be called once a none image file has been downloaded (probably JSON or
 * HTML)
 * @param content
 */
void MainWindow::slotDownloadComplete(QString content)
{
    printLine("Daten geladen - Ermittle URLs");
    QList<ImageItem> itemList = getItemList(content.toUtf8());
    qDebug() << "Anzahl:" << itemList.size();
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
                printLine("Download landscape image");
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
    QSettings   settings(C_MW_IniFile, QSettings::NativeFormat);
    settings.setValue("orientation", ui->cmbOrientation->currentIndex());
    settings.setValue("title",  ui->cmbTitle->currentIndex());
    settings.setValue("geometry", this->geometry());

}

void MainWindow::loadSettings(void)
{
    QSettings   settings(C_MW_IniFile, QSettings::NativeFormat);
    ui->cmbOrientation->setCurrentIndex(settings.value("orientation", 0).toInt());
    ui->cmbTitle->setCurrentIndex(settings.value("title", 0).toInt());
    QRect rect = settings.value("geometry").toRect();
    this->setGeometry(rect);
}

QList<ImageItem> MainWindow::getItemList(QByteArray data)
{
    return m_currProv->getItemList(data);
}

QString MainWindow::createFirstRequest(void)
{
    if (m_provBing->canCreateNextRequest())
        m_currProv = m_provBing;
    else m_currProv = m_provSpot;
    int index = random() % 6;
    if (index < 3)
        m_currProv = m_provCast;
    else m_currProv = m_provSpot;
    return m_currProv->createFirstRequest();
}

void MainWindow::createCacheDirs(void)
{
    QString path = QString("download") + QDir::separator() + "spotlight";
    QDir md = QDir();
    md.mkpath(path + QDir::separator() + "portrait");
    md.mkpath(path + QDir::separator() + "landscape");
    path = QString("download") + QDir::separator() + "bing";
    md.mkpath(path + QDir::separator() + "landscape");
    md.mkpath(path + QDir::separator() + "portrait");
    path = QString("download") + QDir::separator() + "chromecast";
    md.mkpath(path + QDir::separator() + "landscape");
    md.mkpath(path + QDir::separator() + "portrait");
}

void MainWindow::slotImageDownloadComplete(ImageItem item)
{
    QString filename = item.filename();
    item.image().save(filename);
    m_database.addImage(item);
    printLine("Runter geladen:" + item.title());
    m_addThread->doAddImage(item, true);
}

void MainWindow::slotAddImage(QTableWidgetItem *item, int row, int col, int height)
{
    if (row+1 >= ui->tbwOverview->rowCount())
        ui->tbwOverview->setRowCount(row+1);
    ui->tbwOverview->setItem(row, col, item);
    ui->tbwOverview->setRowHeight(row, height+8);
    ui->tbwOverview->setColumnWidth(col, 160);
    ui->tbwOverview->scrollToBottom();
}

void MainWindow::slotDownloadsFinished(void)
{
}

/*
    Create absolute path to image. This is needed to set the wallpaper
    in KDE.
 */
QString MainWindow::createStoredImageFilename(ImageItem &img)
{
    QString fname = QDir::currentPath() + QDir::separator() + img.filename();
    return fname;
}

void MainWindow::slotContextMenuRequested(const QPoint pos)
{
    QTableWidgetItem *item = ui->tbwOverview->currentItem();
    if (item == NULL)
        return;
    int index = item->data(Qt::UserRole +1).toInt();
    ImageItem img = m_addThread->getItem(index);
    if (img.image().isNull())
        return;
    markTagSelection(m_tags, img);
    QAction *action = m_contextMenu->exec(ui->tbwOverview->viewport()->mapToGlobal(pos));
    if (action == NULL)
        return;
    QString fname = createStoredImageFilename(img);
    if (action->text() == tr("Set as Background"))
    {
#ifdef Q_OS_LINUX        
        FILE *file = fopen("/tmp/change.sh", "w");

        if (file != NULL)
        {
            fprintf(file, "qdbus ");
            fprintf(file, "org.kde.plasmashell ");
            fprintf(file, "/PlasmaShell ");
            fprintf(file, "org.kde.PlasmaShell.evaluateScript ");
            fprintf(file, "'var allDesktops = desktops();print (allDesktops);for (i=0;i<allDesktops.length;i++)");
            fprintf(file, "{d = allDesktops[i];d.wallpaperPlugin = \"org.kde.image\";");
            fprintf(file, "d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");");
            fprintf(file, "d.writeConfig(\"Image\", \"file://%s\")}'", fname.toStdString().c_str() );
            fclose(file);
            QFile file("/tmp/change.sh");
            file.setPermissions(QFileDevice::ExeOwner | QFileDevice::ReadOwner | QFileDevice::WriteOwner);
            QProcess::execute("/tmp/change.sh");
        }
        QString name = QDir::tempPath() + QDir::separator() + "change.sh";
        // For gnome, this may work
        // gsettings set org.gnome.desktop.background picture-uri file:///absolute/path/to/picture.jpg
#endif
//        qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript 'var allDesktops = desktops();print (allDesktops);for (i=0;i<allDesktops.length;i++) {d = allDesktops[i];d.wallpaperPlugin = "org.kde.image";d.currentConfigGroup = Array("Wallpaper", "org.kde.image", "General");d.writeConfig("Image", "file://download/spotlight/landscape/landscape_SlangkopLeuchtturmKommetjie_Südafrika.jpg")}'

    }
    else
    if (action->text() == tr("Show"))
    {
        m_changeImgTimeout.stop();
        m_title = "";
        if (m_imgNew.load(fname))
        {
            m_title = img.title();
            this->setFixedSize(m_imgNew.width(), m_imgNew.height());
            this->centralWidget()->repaint();
            clickedHideGUI();
        }
        m_changeImgTimeout.start(C_MW_TimeOut);

    }
}

void MainWindow::slotTagged(void)
{
    QAction *act    = static_cast<QAction*>(sender());
    int  tagid   = act->data().toInt();
    bool checked = act->isChecked();
    QTableWidgetItem *item = ui->tbwOverview->currentItem();
    if (item == NULL)
        return;
    int index = item->data(Qt::UserRole +1).toInt();
    ImageItem img = m_addThread->getItem(index);
    m_database.tagImage(checked, tagid, img.id());
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    if (!m_imgOld.isNull())
    {
        paint.setOpacity(1.0);
        paint.drawImage(QPoint(0,0), m_imgOld);
    }
    if (!m_imgNew.isNull())
    {
        paint.setOpacity(m_fade);
        paint.drawImage(QPoint(0,0), m_imgNew);
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
    m_addThread->doInit(Filter::FI_ALL);

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
