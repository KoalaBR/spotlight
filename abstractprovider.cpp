#include "abstractprovider.h"
#include <QDebug>

AbstractProvider::AbstractProvider(QPlainTextEdit *edit)
{
    m_edit = edit;
}

AbstractProvider::~AbstractProvider()
{

}

QString AbstractProvider::createFirstRequest(void)
{
    qDebug() << "Not implemented: getFirstRequest()";
    return "";
}

QString AbstractProvider::createNextRequest(void)
{
    qDebug() << "getNextRequest(): not implemented";
    return "";
}

QList<ImageItem> AbstractProvider::getItemList(QByteArray data)
{
    Q_UNUSED(data);
    qDebug() << "getItemList(): Not implemented";
    QList<ImageItem> list;
    return list;
}

bool AbstractProvider::canCreateNextRequest(void)
{
    return true;
}

void AbstractProvider::printLine(QString line)
{
    m_edit->moveCursor(QTextCursor::Start);
    m_edit->insertPlainText(line+"\r");
}

