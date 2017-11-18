#ifndef ABSTRACTPROVIDER_H
#define ABSTRACTPROVIDER_H

#include <QList>
#include <QString>
#include <QPlainTextEdit>

#include "imageitem.h"

class AbstractProvider
{
public:
    AbstractProvider(QPlainTextEdit *edit);
    virtual ~AbstractProvider();
    virtual QString             createFirstRequest(void);
    virtual QString             createNextRequest(void);
    virtual QList<ImageItem>    getItemList(QByteArray data);
    virtual bool                canCreateNextRequest(void);
protected:
    void printLine(QString line);
private:
    QPlainTextEdit  *m_edit;
};

#endif // ABSTRACTPROVIDER_H
