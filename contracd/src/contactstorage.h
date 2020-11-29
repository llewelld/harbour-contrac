#ifndef CONTACTSTORAGE_H
#define CONTACTSTORAGE_H

#include <QObject>
#include <QFile>

#include "contactmatch.h"
#include "diagnosiskey.h"

// Amount of data to store
#define DAYS_TO_STORE (15)

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
    QList<RpiDataItem> findRpiMatches(quint32 day, QList<QByteArray> const &rpis);
    QList<ContactMatch> findDtkMatches(quint32 day, QList<DiagnosisKey> const &dtks);

signals:

public slots:
    Q_INVOKABLE void addContact(ctinterval interval, const QByteArray &rpi, const QByteArray &aem, qint16 rssi);
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
