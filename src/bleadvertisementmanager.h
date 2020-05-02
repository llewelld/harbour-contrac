/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#ifndef BLEADVERTISEMENTMANAGER_H
#define BLEADVERTISEMENTMANAGER_H

#include <QObject>
#include <QDBusInterface>

class BleAdvertisement;

class BleAdvertisementManager : public QObject
{
    Q_OBJECT
public:
    explicit BleAdvertisementManager(QObject *parent = nullptr);
    ~BleAdvertisementManager();

    void connectDBus();
    void registerAdvertisement(BleAdvertisement * advertisement);
    void unRegisterAdvertisement(BleAdvertisement * advertisement);

public slots:
    void onPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);

private:
    QDBusInterface *m_interface;
};

#endif // BLEADVERTISEMENTMANAGER_H
