#ifndef DAYSTORAGE_H
#define DAYSTORAGE_H

#include <QObject>
#include <QFile>
#include <QMutex>

#include "contactmatch.h"
#include "diagnosiskey.h"

class BloomFilter;

class DayStorage : public QObject
{
    Q_OBJECT
public:
    explicit DayStorage(quint32 day, QObject * parent = nullptr);
    ~DayStorage();

    QList<RpiDataItem> findRpiMatches(QList<QByteArray> const &rpis);
    QList<ContactMatch> findDtkMatches(QList<DiagnosisKey> const &dtks);

    void addContact(ctinterval interval, const QByteArray &rpi, const QByteArray &aem, qint16 rssi);
    void load();
    void save();
    quint32 dayNumber();

    void dumpData();

private:
    void findProbables(const QList<DiagnosisKey> &dtks, QList<ContactMatch>& probables);
    void findActuals(const QList<ContactMatch>& probables, QList<ContactMatch> &actuals);
    bool probableMatch(const QByteArray &rpi);

private:
    quint32 m_day;
    QFile m_contacts;
    BloomFilter *m_filter;
    bool m_filter_changed;
    QMutex m_contactsMutex;
    QMutex m_filterMutex;
};

#endif // DAYSTORAGE_H
