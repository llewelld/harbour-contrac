#include <QStandardPaths>
#include <QDir>
#include <QByteArray>
#include <QDebug>

#include "contrac.h"
#include "bloomfilter.h"

#include "contactstorage.h"

// Optimised for 2048 entries
#define BLOOM_FILTER_SIZE (32768)
#define BLOOM_FILTER_HASHES (11)

ContactStorage::ContactStorage(Contrac *parent)
    : m_contrac(parent)
{
    bool result;

    QDir dir;
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    dir.mkpath(folder);
    qDebug() << "Storage: " << folder;

    quint32 dayNumber = m_contrac->dayNumber();
    QString leafName = QStringLiteral("%1.dat");
    leafName = leafName.arg(dayNumber, 8, 16, QLatin1Char('0'));
    m_today.setFileName(folder + "/" + leafName);
    qDebug() << "File: " << m_today.fileName();
    result = m_today.open(QIODevice::ReadWrite);
    if (result) {
        m_today.seek(m_today.size());
    }
    else {
        qDebug() << "Error opening file to write: " << m_today.fileName();
    }

    // Read in the bloom filter
    m_filter = new BloomFilter();
    leafName = QStringLiteral("%1.bloom");
    leafName = leafName.arg(dayNumber, 8, 16, QLatin1Char('0'));
    QFile filter;
    filter.setFileName(folder + "/" + leafName);
    qDebug() << "File: " << filter.fileName();
    result = filter.open(QIODevice::ReadOnly);
    if (result) {
        QByteArray data = filter.readAll();
        // Preinitalised Bloomfilter
        m_filter->setFilter(data, 6);
    } else {
        // Empty Bloomfilter
        m_filter->clear(BLOOM_FILTER_SIZE, BLOOM_FILTER_HASHES);
        qDebug() << "Error opening file to write: " << filter.fileName();
    }
}

ContactStorage::~ContactStorage()
{
    int result;

    m_today.close();

    // Write out the bloom filter
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    quint32 dayNumber = m_contrac->dayNumber();
    QString leafName = QStringLiteral("%1.bloom");
    leafName = leafName.arg(dayNumber, 8, 16, QLatin1Char('0'));
    QFile filter;
    filter.setFileName(folder + "/" + leafName);
    result = filter.open(QIODevice::WriteOnly);
    if (result) {
        QByteArray const &data = m_filter->getFilter();
        filter.write(data);
    }
    else {
        qDebug() << "Error opening file to write: " << filter.fileName();
    }
}

void ContactStorage::addContact(const QByteArray &rpi, qint16 rssi)
{
    quint8 interval;
    QByteArray data(rpi);

    interval = m_contrac->timeIntervalNumber();

    data += QByteArray((char const *)&interval, sizeof(interval));
    data += QByteArray((char const *)&rssi, sizeof(rssi));

    m_today.write(data);
    m_filter->add(rpi);
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

bool ContactStorage::probableMatch(const QByteArray &rpi)
{
    return m_filter->test(rpi);
}
