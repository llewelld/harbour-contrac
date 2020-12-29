#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "rpidataitem.h"
#include "bloomfilter.h"
#include "contrac.h"
#include "contactmatch.h"
#include "metadata.h"

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

QList<RpiDataItem> DayStorage::findRpiMatches(QList<QByteArray> const &rpis)
{
    bool result;
    QFile captures;
    QString leafname;

    m_filterMutex.lock();
    QList<QByteArray> probables;
    for (QByteArray const &rpi : rpis) {
        if (m_filter->test(rpi)) {
            probables.append(rpi);
        }
    }
    m_filterMutex.unlock();

    QList<RpiDataItem> actuals;

    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";

    m_contactsMutex.lock();
    leafname = QStringLiteral("%1.dat");
    leafname = leafname.arg(m_day, 8, 16, QLatin1Char('0'));
    captures.setFileName(folder + "/" + leafname);
    result = captures.open(QIODevice::ReadOnly);
    if (result) {
        QByteArray capture = captures.read(RPI_SERIALISE_SIZE);
        while (!capture.isEmpty() && !probables.isEmpty()) {
            RpiDataItem rpi;
            result = rpi.deserialise(capture);
            if (result) {
                int pos = 0;
                while (pos < probables.size()) {
                    if (rpi.m_rpi == probables.at(pos)) {
                        actuals.append(rpi);
                        probables.removeAt(pos);
                    }
                    else {
                        ++pos;
                    }
                }
            }
            capture = captures.read(RPI_SERIALISE_SIZE);
        }

    }
    else {
        qDebug() << "Error opening data file: " << captures.fileName();
    }
    m_contactsMutex.unlock();

    return actuals;
}

QList<ContactMatch> DayStorage::findDtkMatches(QList<DiagnosisKey> const &dtks)
{
    bool result;
    QFile captures;
    QString leafname;
    QList<ContactMatch> probables;
    ContactMatch probable;
    QList<QList<RpiDataItem>> actuals;

    // Generate all the rpis from the dtks and do a quick pass with the Bloom filter
    for (DiagnosisKey const &dtk : dtks) {
        quint8 startTime = dtk.m_rollingStartIntervalNumber % 144;
        quint8 endTime;
        if (dtk.m_rollingPeriod - startTime > 144) {
            endTime = 144 - startTime;
        }
        else {
            endTime = startTime + static_cast<quint8>(dtk.m_rollingPeriod);
        }
        m_filterMutex.lock();
        // Generate up to 144 rpis for each dtk
        for (quint8 interval = startTime; interval < endTime; ++interval) {
            QByteArray rpi(Contrac::randomProximityIdentifier(dtk.m_dtk, interval));
            if (m_filter->test(rpi)) {
                // Probable match
                probable.m_dtk = &dtk;
                ctinterval quantised = intervalToCtInterval(interval);
                probable.m_rpis.append(RpiDataItem(quantised, 0, rpi, QByteArray(AEM_SIZE, '\0')));
            }
        }
        m_filterMutex.unlock();
        if (probable.m_rpis.size() > 0) {
            probables.append(probable);
            actuals.append(QList<RpiDataItem>());
        }
    }
    qDebug() << "Probables size: " << probables.size();

    // There's no point opening the captures file unless there are some probables to check
    if (!probables.isEmpty()) {
        // Now go through in detail and filter out any errors
        // Adding in rssi values as we go
        m_contactsMutex.lock();
        QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
        leafname = QStringLiteral("%1.dat");
        leafname = leafname.arg(m_day, 8, 16, QLatin1Char('0'));
        captures.setFileName(folder + "/" + leafname);
        result = captures.open(QIODevice::ReadOnly);
        if (result) {
            QByteArray capture = captures.read(RPI_SERIALISE_SIZE);
            while (!capture.isEmpty() && !probables.isEmpty()) {
                RpiDataItem rpi;
                result = rpi.deserialise(capture);
                if (result) {
                    // Cycle through the dtks
                    int dtk_pos = 0;
                    while (dtk_pos < probables.size()) {
                        // Cycle through the rpis associated with this dtk
                        int rpi_pos = 0;
                        ContactMatch &probable = probables[dtk_pos];
                        while (rpi_pos < probable.m_rpis.size()) {
                            RpiDataItem &probable_rpi = probable.m_rpis[rpi_pos];
                            if ((rpi.m_rpi == probable_rpi.m_rpi) && ctIntervalsMatch(rpi.m_interval, probable_rpi.m_interval)) {
                                // We have a match!
                                // There could be multiple beacons matching this rpi, so we can't remove it yet
                                actuals[dtk_pos].append(rpi);
                            }
                            else {
                                if (rpi.m_rpi == probable_rpi.m_rpi) {
                                    qDebug() << "Failed due to time mismatch";
                                    qDebug() << "Beacon time: " << rpi.m_interval;
                                    qDebug() << "Diagnosis time: " << probable_rpi.m_interval;
                                }
                            }
                            ++rpi_pos;
                        }
                        ++dtk_pos;
                    }
                }
                capture = captures.read(RPI_SERIALISE_SIZE);
            }

            captures.close();

            // Remove empty dtks and transfer actuals
            int dtk_pos = 0;
            while (dtk_pos < probables.size()) {
                if (actuals[dtk_pos].size() > 0) {
                    probables[dtk_pos].m_rpis = actuals[dtk_pos];
                    ++dtk_pos;
                }
                else{
                    probables.removeAt(dtk_pos);
                    actuals.removeAt(dtk_pos);
                }
            }
        }
        else {
            qDebug() << "Error opening data file: " << captures.fileName();
        }

        m_contactsMutex.unlock();
    }
    qDebug() << "Actuals size: " << probables.size();

    return probables;
}

void DayStorage::addContact(ctinterval interval, const QByteArray &rpi, const QByteArray &aem, qint16 rssi)
{
    RpiDataItem dataItem(interval, rssi, rpi, aem);
    QByteArray data(rpi);

    data = dataItem.serialise();
    if (!data.isEmpty()) {
        m_contactsMutex.lock();
        qDebug() << "Outputting to file: " << m_contacts.fileName();
        m_contacts.write(data);
        m_contactsMutex.unlock();

        m_filterMutex.lock();
        m_filter->add(rpi);
        m_filter_changed = true;
        m_filterMutex.unlock();
    }
    else {
        qDebug() << "Contact could not be stored: incorrect data size";
    }
}

void DayStorage::load()
{
    bool result;
    QString leafname;
    QDir dir;

    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    dir.mkpath(folder);

    m_contactsMutex.lock();
    leafname = QStringLiteral("%1.dat");
    leafname = leafname.arg(m_day, 8, 16, QLatin1Char('0'));
    m_contacts.setFileName(folder + "/" + leafname);
    result = m_contacts.open(QIODevice::ReadWrite);
    if (result) {
        m_contacts.seek(m_contacts.size());
    }
    else {
        qDebug() << "Error opening file to write: " << m_contacts.fileName();
    }
    m_contactsMutex.unlock();

    m_filterMutex.lock();
    // Save out the old bloom filter
    if (m_filter) {
        m_filter->save();
        delete m_filter;
    }
    // Read in the new bloom filter
    m_filter_changed = false;
    m_filter = new BloomFilter(m_day);
    result = m_filter->load(m_day);
    if (!result) {
        // Empty Bloomfilter
        m_filter->clear(BLOOM_FILTER_SIZE, BLOOM_FILTER_HASHES);
        m_filter->setDay(m_day);
    }
    m_filterMutex.unlock();
}

void DayStorage::save()
{
    m_filterMutex.lock();
    if (m_filter_changed) {
        // Write out the bloom filter
        m_filter->save();
        m_filter_changed = false;
    }
    m_filterMutex.unlock();
}

bool DayStorage::probableMatch(const QByteArray &rpi)
{
    QMutexLocker locker(&m_filterMutex);
    return m_filter->test(rpi);
}

quint32 DayStorage::dayNumber()
{
    QMutexLocker locker(&m_contactsMutex);
    return m_day;
}

void DayStorage::dumpData()
{
    bool result;

    m_contactsMutex.lock();
    m_contacts.seek(0);
    QByteArray read = m_contacts.read(RPI_SERIALISE_SIZE);
    while (!read.isEmpty()) {
        RpiDataItem rpi;
        result = rpi.deserialise(read);
        if (result) {
            qDebug() << rpi.m_rpi.toHex() << ", " << rpi.m_aem.toHex() << "," << rpi.m_interval << ", " << rpi.m_rssi;
        }
        read = m_contacts.read(RPI_SERIALISE_SIZE);
    }
    m_contactsMutex.unlock();
}

