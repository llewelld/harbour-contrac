#ifndef RPIDATAITEM_H
#define RPIDATAITEM_H

#include <QObject>

#include "contactinterval.h"
#include "metadata.h"

#define RPI_SERIALISE_SIZE (RPI_SIZE + AEM_SIZE + sizeof(RpiDataItem::m_interval) + sizeof(RpiDataItem::m_rssi))

class RpiDataItem
{
public:
    RpiDataItem();
    RpiDataItem(ctinterval interval, qint16 rssi, QByteArray const &rpi, QByteArray const &aem);

    ctinterval m_interval = 0;
    qint16 m_rssi = 0;
    QByteArray m_rpi;
    QByteArray m_aem;

    QByteArray serialise() const;
    bool deserialise(QByteArray const &data);
};

#endif // RPIDATAITEM_H
