#ifndef SPOTLIGHTPROVIDER_H
#define SPOTLIGHTPROVIDER_H

#include "abstractprovider.h"

class SpotlightProvider : public AbstractProvider
{
public:
    SpotlightProvider(QPlainTextEdit *edit);
    QString             createFirstRequest(void);
    QList<ImageItem>    getItemList(QByteArray data);

private:
    QList<ImageItem> decodeJsonList(QString value);
    QList<ImageItem> getImageItem(QJsonObject image);
    QString     m_locale;               ///< en-EN, de-DE, changes the language of the title
};

#endif // SPOTLIGHTPROVIDER_H
