#ifndef CONTACTSTORAGE_H
#define CONTACTSTORAGE_H

#include <QObject>
#include <QFile>

class Contrac;
class DayStorage;

class ContactStorage : public QObject
{
    Q_OBJECT

public:
    explicit ContactStorage(Contrac *parent = nullptr);
    ~ContactStorage();

    void rotateData();
    static void clearAllDataFiles();
    void harvestOldData();
    QByteArrayList findRpiMatches(quint32 day, QByteArrayList rpis);
    QByteArrayList findDtkMatches(quint32 day, QByteArrayList dtks);

signals:

public slots:
    Q_INVOKABLE void addContact(quint8 interval, const QByteArray &rpi, qint16 rssi);
    void dumpData(quint32 day = 0);
    void onTimeChanged();

private:
    DayStorage * getStorage(quint32 day);

private:
    Contrac *m_contrac;
    DayStorage *m_today;
    DayStorage *m_other;
};

#endif // CONTACTSTORAGE_H
