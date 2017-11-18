#ifndef BINGPROVIDER_H
#define BINGPROVIDER_H
#include <QPlainTextEdit>

#include "abstractprovider.h"

class BingProvider : public AbstractProvider
{
public:
    BingProvider(QPlainTextEdit *edit);
    QString             createFirstRequest(void);
    bool                canCreateNextRequest(void);
    QList<ImageItem>    getItemList(QByteArray data);

private:
    bool        m_nextRequest;
};

#endif // BINGPROVIDER_H
