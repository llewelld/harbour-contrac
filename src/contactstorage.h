#ifndef CONTACTSTORAGE_H
#define CONTACTSTORAGE_H

#include <QObject>
#include <QFile>

class Contrac;
class BloomFilter;

class ContactStorage : public QObject
{
    Q_OBJECT
public:
    explicit ContactStorage(Contrac *parent = nullptr);
    ~ContactStorage();

signals:

public slots:
    Q_INVOKABLE void addContact(const QByteArray &rpi, qint16 rssi);
    void dumpData();
    Q_INVOKABLE bool probableMatch(const QByteArray &rpi);

private:
    QFile m_today;
    Contrac *m_contrac;
    BloomFilter *m_filter;
};

#endif // CONTACTSTORAGE_H
