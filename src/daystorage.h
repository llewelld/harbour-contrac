#ifndef DAYSTORAGE_H
#define DAYSTORAGE_H

#include <QObject>
#include <QFile>

class BloomFilter;

class DayStorage : public QObject
{
    Q_OBJECT
public:
    explicit DayStorage(quint32 day, QObject * parent);
    ~DayStorage();

    QByteArrayList findRpiMatches(QByteArrayList rpis);
    QByteArrayList findDtkMatches(QByteArrayList dtks);

    void addContact(quint8 interval, const QByteArray &rpi, qint16 rssi);
    void load();
    void save();
    quint32 dayNumber() const;

    void dumpData();

private:
    bool probableMatch(const QByteArray &rpi);

private:
    quint32 m_day;
    QFile m_contacts;
    BloomFilter *m_filter;
    bool m_filter_changed;
};

#endif // DAYSTORAGE_H
