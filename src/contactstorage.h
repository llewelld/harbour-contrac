#ifndef CONTACTSTORAGE_H
#define CONTACTSTORAGE_H

#include <QObject>
#include <QFile>

class Contrac;
class BloomFilter;

class ContactStorage : public QObject
{
    Q_OBJECT

    class RpiDateItem
    {
    public:
        quint8 m_interval = 0;
        qint16 m_rssi = 0;
        QByteArray m_rpi;

        QByteArray serialise() const;
        bool deserialise(QByteArray const &data);
    };

public:
    explicit ContactStorage(Contrac *parent = nullptr);
    ~ContactStorage();

    void rotateData();
    QByteArrayList findMatches(quint32 day, QByteArrayList rpis);

signals:

public slots:
    Q_INVOKABLE void addContact(const QByteArray &rpi, qint16 rssi);
    void dumpData();

private:
    bool probableMatch(const QByteArray &rpi, quint32 day = 0);
    void harvestOldData();

private:
    QFile m_today;
    Contrac *m_contrac;
    BloomFilter *m_filterToday;
    BloomFilter *m_filterOther;
};

#endif // CONTACTSTORAGE_H
