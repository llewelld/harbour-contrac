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

// Amount of data to store
#define DAYS_TO_STORE (14)

ContactStorage::ContactStorage(Contrac *parent)
    : m_contrac(parent)
    , m_filterToday(nullptr)
    , m_filterOther(nullptr)
{
    bool result;
    QString leafname;
    QDir dir;

    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    dir.mkpath(folder);
    qDebug() << "Storage: " << folder;

    quint32 dayNumber = m_contrac->dayNumber();
    leafname = QStringLiteral("%1.dat");
    leafname = leafname.arg(dayNumber, 8, 16, QLatin1Char('0'));
    m_today.setFileName(folder + "/" + leafname);
    qDebug() << "File: " << m_today.fileName();
    result = m_today.open(QIODevice::ReadWrite);
    if (result) {
        m_today.seek(m_today.size());
    }
    else {
        qDebug() << "Error opening file to write: " << m_today.fileName();
    }

    // Read in the bloom filter
    m_filterToday = new BloomFilter(dayNumber);
    m_filterToday->load(dayNumber);
    if (!result) {
        // Empty Bloomfilter
        m_filterToday->clear(BLOOM_FILTER_SIZE, BLOOM_FILTER_HASHES);
        qDebug() << "Could not open Bloom Filter file for : " << dayNumber;
    }

    m_filterOther = new BloomFilter();
}

ContactStorage::~ContactStorage()
{
    m_today.close();

    // Write out the bloom filter
    m_filterToday->save();

    delete m_filterOther;
    delete (m_filterOther);
}

void ContactStorage::addContact(const QByteArray &rpi, qint16 rssi)
{
    quint8 interval;
    QByteArray data(rpi);

    interval = m_contrac->timeIntervalNumber();

    data += QByteArray((char const *)&interval, sizeof(interval));
    data += QByteArray((char const *)&rssi, sizeof(rssi));

    m_today.write(data);
    m_filterToday->add(rpi);
}

void ContactStorage::dumpData()
{
    bool result;
    m_today.seek(0);
    QByteArray read = m_today.read(16 + 1 + 2);
    while (!read.isEmpty()) {
        RpiDateItem rpi;
        result = rpi.deserialise(read);
        if (result) {
            qDebug() << rpi.m_rpi.toHex() << ", " << rpi.m_interval << ", " << rpi.m_rssi;
        }
        read = m_today.read(16 + 1 + 2);
    }
}

void ContactStorage::rotateData()
{
}

QByteArrayList ContactStorage::findMatches(quint32 day, QByteArrayList rpis)
{
    BloomFilter * filter;
    QFile captures;
    QString leafname;
    bool result;

    if (day == m_contrac->dayNumber()) {
        filter = m_filterToday;
    }
    else {
        if (m_filterOther->day() != day) {
            m_filterOther->load(day);
        }
        filter = m_filterOther;
    }


    QByteArrayList probables;
    for (QByteArray rpi : rpis) {
        if (filter->test(rpi)) {
            probables.append(rpis);
        }
    }

    QByteArrayList actuals;

    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";

    leafname = QStringLiteral("%1.dat");
    leafname = leafname.arg(day, 8, 16, QLatin1Char('0'));
    captures.setFileName(folder + "/" + leafname);
    result = captures.open(QIODevice::ReadOnly);
    if (result) {
        QByteArray capture = captures.read(16 + 1 + 2);
        while (!capture.isEmpty() && !probables.isEmpty()) {
            RpiDateItem rpi;
            result = rpi.deserialise(capture);
            if (result) {
                int pos = 0;
                while (pos < probables.size()) {
                    if (rpi.m_rpi == probables.at(pos)) {
                        actuals.append(probables.at(pos));
                        probables.removeAt(pos);
                    }
                    else {
                        ++pos;
                    }
                }
            }
            capture = captures.read(16 + 1 + 2);
        }

    }
    else {
        qDebug() << "Error opening data file: " << captures.fileName();
    }

    return actuals;
}

bool ContactStorage::probableMatch(const QByteArray &rpi, quint32 day)
{
    if (day == 0) {
        day = m_contrac->dayNumber();
    }

    if (m_filterOther->day() != day) {
        m_filterOther->load(day);
    }

    return m_filterOther->test(rpi);
}

void ContactStorage::harvestOldData()
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    QDir dataDir;
    quint32 day = m_contrac->dayNumber();
    QFileInfoList files;

    dataDir.setPath(folder);
    dataDir.setFilter(QDir::Files);
    files = dataDir.entryInfoList();
    for (QFileInfo file : files) {
        quint32 fileDay;
        QString extension = file.completeSuffix();
        if (extension == "dat" || extension == "bloom") {
            QString base = file.baseName();
            bool ok;
            fileDay = base.toUInt(&ok, 16);
            if (ok && (fileDay < (day - DAYS_TO_STORE))) {
                dataDir.remove(file.fileName());
            }
        }
    }
}

QByteArray ContactStorage::RpiDateItem::serialise() const
{
    QByteArray data(m_rpi);

    data += QByteArray((char const *)&m_interval, sizeof(m_interval));
    data += QByteArray((char const *)&m_rssi, sizeof(m_rssi));

    return data;
}

bool ContactStorage::RpiDateItem::deserialise(QByteArray const &data)
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


