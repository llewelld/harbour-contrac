#include <QDateTime>

#include "contactmodel.h"

#define CONTACT_LIFETIME (45)

ContactModel::ContactModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roles[ContactAddress] = "address";
    m_roles[ContactRpi] = "rpi";
    m_roles[ContactRssi] = "rssi";
}

ContactModel::~ContactModel()
{
}

QHash<int, QByteArray> ContactModel::roleNames() const
{
    return m_roles;
}

void ContactModel::addContact(const QString &address, const QByteArray &rpi, qint16 rssi)
{
    Contact contact;

    contact.address = address;
    contact.rpi = rpi;
    contact.rssi = rssi;
    contact.added = QDateTime::currentDateTime();

    beginInsertRows(QModelIndex(), 0, 0);
    m_contacts.prepend(contact);
    endInsertRows();
}

int ContactModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_contacts.count();
}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > m_contacts.count())
        return QVariant();

    const Contact &contact = m_contacts[index.row()];
    if (role == ContactAddress)
        return contact.address;
    else if (role == ContactRpi)
        return contact.rpi;
    else if (role == ContactRssi)
        return contact.rssi;
    return QVariant();
}

void ContactModel::harvestOldContacts()
{
    int pos;
    pos = 0;
    while (pos < m_contacts.count()) {
        Contact contact = m_contacts[pos];
        if (contact.added.addSecs(CONTACT_LIFETIME) < QDateTime::currentDateTime()) {
            beginRemoveRows(QModelIndex(), pos, pos);
            m_contacts.removeAt(pos);
            endRemoveRows();
        }
        else {
            ++pos;
        }
    }
}
