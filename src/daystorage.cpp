#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "rpidataitem.h"
#include "bloomfilter.h"
#include "contrac.h"

#include "daystorage.h"

// Optimised for 2048 entries
#define BLOOM_FILTER_SIZE (32768)
#define BLOOM_FILTER_HASHES (11)

DayStorage::DayStorage(quint32 day, QObject * parent)
    : QObject(parent)
    , m_day(day)
    , m_filter(nullptr)
    , m_filter_changed(false)
{
        load();
}

DayStorage::~DayStorage()
{
    m_contacts.close();
    save();

    delete m_filter;
}

QByteArrayList DayStorage::findRpiMatches(QByteArrayList rpis)
{
    bool result;
    QFile captures;
    QString leafname;

    QByteArrayList probables;
    for (QByteArray rpi : rpis) {
        if (m_filter->test(rpi)) {
            probables.append(rpis);
        }
    }

    QByteArrayList actuals;

    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";

    leafname = QStringLiteral("%1.dat");
    leafname = leafname.arg(m_day, 8, 16, QLatin1Char('0'));
    captures.setFileName(folder + "/" + leafname);
    result = captures.open(QIODevice::ReadOnly);
    if (result) {
        QByteArray capture = captures.read(16 + 1 + 2);
        while (!capture.isEmpty() && !probables.isEmpty()) {
            RpiDataItem rpi;
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

QByteArrayList DayStorage::findDtkMatches(QByteArrayList dtks)
{
    // Generate 144 rpis for each dtk
    QByteArrayList rpis;
    QByteArray rpi;

    for (QByteArray const &dtk : dtks) {
        for (quint8 time_interaval_number = 0; time_interaval_number < 144; ++time_interaval_number) {
            rpi = Contrac::randomProximityIdentifier(dtk, time_interaval_number);
            rpis.append(rpi);
        }
    }

    return findRpiMatches(rpis);
}

void DayStorage::addContact(quint8 interval, const QByteArray &rpi, qint16 rssi)
{
    QByteArray data(rpi);

    data += QByteArray((char const *)&interval, sizeof(interval));
    data += QByteArray((char const *)&rssi, sizeof(rssi));

    m_contacts.write(data);
    m_filter->add(rpi);
    m_filter_changed = true;
}

void DayStorage::load()
{
    bool result;
    QString leafname;
    QDir dir;

    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    dir.mkpath(folder);
    qDebug() << "Storage: " << folder;

    leafname = QStringLiteral("%1.dat");
    leafname = leafname.arg(m_day, 8, 16, QLatin1Char('0'));
    m_contacts.setFileName(folder + "/" + leafname);
    qDebug() << "File: " << m_contacts.fileName();
    result = m_contacts.open(QIODevice::ReadWrite);
    if (result) {
        m_contacts.seek(m_contacts.size());
    }
    else {
        qDebug() << "Error opening file to write: " << m_contacts.fileName();
    }

    // Read in the bloom filter
    if (m_filter) {
        m_filter->save();
        delete m_filter;
    }
    m_filter_changed = false;
    m_filter = new BloomFilter(m_day);
    result = m_filter->load(m_day);
    if (!result) {
        // Empty Bloomfilter
        m_filter->clear(BLOOM_FILTER_SIZE, BLOOM_FILTER_HASHES);
        m_filter->setDay(m_day);
        qDebug() << "Could not open Bloom Filter file for : " << m_day;
    }
}

void DayStorage::save()
{
    if (m_filter_changed) {
        // Write out the bloom filter
        m_filter->save();
        m_filter_changed = false;
    }
}

bool DayStorage::probableMatch(const QByteArray &rpi)
{
    return m_filter->test(rpi);
}

quint32 DayStorage::dayNumber() const
{
    return m_day;
}

void DayStorage::dumpData()
{
    bool result;
    m_contacts.seek(0);
    QByteArray read = m_contacts.read(16 + 1 + 2);
    while (!read.isEmpty()) {
        RpiDataItem rpi;
        result = rpi.deserialise(read);
        if (result) {
            qDebug() << rpi.m_rpi.toHex() << ", " << rpi.m_interval << ", " << rpi.m_rssi;
        }
        read = m_contacts.read(16 + 1 + 2);
    }
}

