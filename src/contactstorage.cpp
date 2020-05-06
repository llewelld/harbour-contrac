#include <QStandardPaths>
#include <QDir>
#include <QByteArray>
#include <QDebug>

#include "contrac.h"

#include "contactstorage.h"

ContactStorage::ContactStorage(Contrac *parent)
    : m_contrac(parent)
{
    bool result;

    QDir dir;
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    dir.mkpath(folder);

    quint32 dayNumber = m_contrac->dayNumber();
    QString leafName = QStringLiteral("%1.dat");
    leafName = leafName.arg(dayNumber, 8, 16, QLatin1Char('0'));
    m_today.setFileName(folder + "/" + leafName);
    qDebug() << "File: " << m_today.fileName();
    result = m_today.open(QIODevice::ReadWrite);
    if (!result) {
        qDebug() << "Error opening file to write: " << m_today.fileName();
    }
    m_today.seek(m_today.size());
}

ContactStorage::~ContactStorage()
{
    m_today.close();
}

void ContactStorage::addContact(const QByteArray &rpi, qint16 rssi)
{
    quint8 interval;
    QByteArray data(rpi);

    interval = m_contrac->timeIntervalNumber();

    data += QByteArray((char const *)&interval, sizeof(interval));
    data += QByteArray((char const *)&rssi, sizeof(rssi));

    m_today.write(data);
}

void ContactStorage::dumpData()
{
    m_today.seek(0);
    QByteArray read = m_today.read(16 + 1 + 2);
    while (!read.isEmpty()) {
        quint8 interval = *(quint8*)(read.mid(16, 1).data());
        qint16 rssi = *(qint16*)(read.mid(16 + 1, 2).data());
        QByteArray const rpi = read.mid(0, 16);
        qDebug() << rpi.toHex() << ", " << interval << ", " << rssi;
        read = m_today.read(16 + 1 + 2);
    }
}
