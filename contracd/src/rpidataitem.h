#ifndef RPIDATAITEM_H
#define RPIDATAITEM_H

#include <QObject>

#include "contactinterval.h"

#define RPI_SERIALISE_SIZE (RPI_SIZE + sizeof(RpiDataItem::m_interval) + sizeof(RpiDataItem::m_rssi))

class RpiDataItem
{
public:
    RpiDataItem();
    RpiDataItem(ctinterval interval, qint16 rssi, QByteArray rpi);

    ctinterval m_interval = 0;
    qint16 m_rssi = 0;
    QByteArray m_rpi;

    QByteArray serialise() const;
    bool deserialise(QByteArray const &data);
};

#endif // RPIDATAITEM_H
