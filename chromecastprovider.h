#ifndef CHROMECASTPROVIDER_H
#define CHROMECASTPROVIDER_H

#include "abstractprovider.h"

class ChromecastProvider : public AbstractProvider
{
public:
    ChromecastProvider(QPlainTextEdit *edit);

    QString             createFirstRequest(void);
    QList<ImageItem>    getItemList(QByteArray data);
    void                setFormat(Format form);

private:
    Format      m_format;       ///< Look for LANDSCAPE, PORTRAIT images (or both)

};

#endif // CHROMECASTPROVIDER_H
