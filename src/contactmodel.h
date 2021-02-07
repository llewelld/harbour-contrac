#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>

class ContactModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ContactRoles
    {
        ContactAddress = Qt::UserRole + 1,
        ContactRpi,
        ContactRssi
    };

    class Contact
    {
    public:
        QString address;
        QByteArray rpi;
        qint16 rssi;
        QDateTime added;
    };

    explicit ContactModel(QObject *parent = 0);
    ~ContactModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE void addContact(const QString &address, const QByteArray &rpi, qint16 rssi);
    Q_INVOKABLE void harvestOldContacts();

private:
    QHash<int, QByteArray> m_roles;
    QList<Contact> m_contacts;
};

#endif // CONTACTMODEL_H
