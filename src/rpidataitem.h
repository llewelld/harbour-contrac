#ifndef RPIDATAITEM_H
#define RPIDATAITEM_H

#include <QObject>

class RpiDataItem
{
public:
    RpiDataItem();

    quint8 m_interval = 0;
    qint16 m_rssi = 0;
    QByteArray m_rpi;

    QByteArray serialise() const;
    bool deserialise(QByteArray const &data);
};

#endif // RPIDATAITEM_H
