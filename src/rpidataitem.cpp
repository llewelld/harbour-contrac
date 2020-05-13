#include "rpidataitem.h"

RpiDataItem::RpiDataItem()
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

    if (data.size() == 19) {
        m_interval = *(quint8*)(data.mid(16, 1).data());
        m_rssi = *(qint16*)(data.mid(16 + 1, 2).data());
        m_rpi = data.mid(0, 16);
        result = true;
    }

    return result;
}


