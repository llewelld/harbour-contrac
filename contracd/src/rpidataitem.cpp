#include <qdebug.h>
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
    bytes = *reinterpret_cast<T*>(data.mid(pos, sizeof(T)).data());
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

RpiDataItem::RpiDataItem(ctinterval interval, qint16 rssi, QByteArray const &rpi, QByteArray const &aem)
    : m_interval(interval)
    , m_rssi(rssi)
    , m_rpi(rpi)
    , m_aem(aem)
{
}

QByteArray RpiDataItem::serialise() const
{
    QByteArray data;

    data = m_rpi;
    data += m_aem;
    data += QByteArray(reinterpret_cast<char const *>(&m_interval), sizeof(m_interval));
    data += QByteArray(reinterpret_cast<char const *>(&m_rssi), sizeof(m_rssi));

    return data;
}

bool RpiDataItem::deserialise(QByteArray const &data)
{
    bool result = false;
    quint8 pos;

    if (data.size() == RPI_SERIALISE_SIZE) {
        pos = 0;
        m_rpi = read(data, pos, RPI_SIZE);
        m_aem = read(data, pos, AEM_SIZE);
        m_interval = read<ctinterval>(data, pos);
        m_rssi = read<qint16>(data, pos);
        result = true;
    }

    return result;
}


