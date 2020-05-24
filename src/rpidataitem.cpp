#include"contrac.h"

#include "rpidataitem.h"

namespace { // Empty namespace

inline QByteArray read(QByteArray const &data, quint8 &pos, quint8 size)
{
    QByteArray bytes = data.mid(pos, size);
    pos += size;
    return bytes;
}

template<class T>
inline T read(QByteArray const &data, quint8 &pos)
{
    T bytes;
    bytes = *(T*)data.mid(pos, sizeof(T)).data();
    pos += sizeof(T);
    return bytes;
}

} // Empty namespace

RpiDataItem::RpiDataItem()
    : m_interval(0)
    , m_rssi(0)
    , m_rpi()
{
}

RpiDataItem::RpiDataItem(ctinterval interval, qint16 rssi, QByteArray rpi)
    : m_interval(interval)
    , m_rssi(rssi)
    , m_rpi(rpi)
{
}

QByteArray RpiDataItem::serialise() const
{
    QByteArray data(m_rpi);

    data += QByteArray((char const *)&m_interval, sizeof(m_interval));
    data += QByteArray((char const *)&m_rssi, sizeof(m_rssi));

    return data;
}

bool RpiDataItem::deserialise(QByteArray const &data)
{
    bool result = false;
    quint8 pos;

    if (data.size() == RPI_SERIALISE_SIZE) {
        pos = 0;
        m_rpi = read(data, pos, RPI_SIZE);
        m_interval = read<ctinterval>(data, pos);
        m_rssi = read<qint16>(data, pos);
        result = true;
    }

    return result;
}


