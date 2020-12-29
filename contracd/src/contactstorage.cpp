#include <QStandardPaths>
#include <QDir>
#include <QByteArray>
#include <QDebug>

#include "contrac.h"
#include "daystorage.h"
#include "rpidataitem.h"

#include "contactstorage.h"

// Optimised for 2048 entries
#define BLOOM_FILTER_SIZE (32768)
#define BLOOM_FILTER_HASHES (11)

ContactStorage::ContactStorage(Contrac *parent)
    : m_contrac(parent)
    , m_today(nullptr)
    , m_other(nullptr)
{
    quint32 day;

    day = m_contrac->dayNumber();
    m_today = new DayStorage(day, this);
    connect(m_contrac, &Contrac::timeChanged, this, &ContactStorage::onTimeChanged);
}

ContactStorage::~ContactStorage()
{
    if (m_other) {
        delete m_other;
        m_other = nullptr;
    }
}

void ContactStorage::onTimeChanged()
{
    quint32 day;

    day = m_contrac->dayNumber();
    if (day != m_today->dayNumber()) {
        qDebug() << "Storage day set to " << day;
        delete m_today;
        m_today = new DayStorage(day, this);
        harvestOldData();
    }
}

void ContactStorage::addContact(ctinterval interval, const QByteArray &rpi, const QByteArray &aem, qint16 rssi)
{
    m_today->addContact(interval, rpi, aem, rssi);
}

void ContactStorage::dumpData(quint32 day)
{
    DayStorage * storage = getStorage(day);
    storage->dumpData();
}

void ContactStorage::rotateData()
{
}

QList<ContactMatch> ContactStorage::findDtkMatches(quint32 day, QList<DiagnosisKey> const &dtks)
{
    DayStorage * storage = getStorage(day);
    return storage->findDtkMatches(dtks);
}

QList<RpiDataItem> ContactStorage::findRpiMatches(quint32 day, QList<QByteArray> const &rpis)
{
    DayStorage * storage = getStorage(day);
    return storage->findRpiMatches(rpis);
}

DayStorage * ContactStorage::getStorage(quint32 day) {
    DayStorage * storage;
    if (day == m_today->dayNumber()) {
        storage = m_today;
    }
    else {
        if (!m_other || (day != m_other->dayNumber())) {
            if (m_other) {
                delete m_other;
            }
            m_other = new DayStorage(day);
        }
        storage = m_other;
    }
    return storage;
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
                qDebug() << "Harvesting file: " << file.fileName();
                dataDir.remove(file.fileName());
            }
        }
    }
}

void ContactStorage::clearAllDataFiles()
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    QDir dataDir;
    QFileInfoList files;

    dataDir.setPath(folder);
    dataDir.setFilter(QDir::Files);
    files = dataDir.entryInfoList();
    for (QFileInfo file : files) {
        QString extension = file.completeSuffix();
        if (extension == "dat" || extension == "bloom") {
            dataDir.remove(file.fileName());
        }
    }
}

