/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "bleadvertisement.h"
#include "bleadvertisementmanager.h"


class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    Q_INVOKABLE void registerAdvert();
    Q_INVOKABLE void unRegisterAdvert();

    Q_INVOKABLE void setAdvertData(QByteArray const &rpi, QByteArray const &metadata);
    Q_INVOKABLE static QString binaryToHex(QByteArray binary, int lineLength);

    bool active() const;

signals:
    void activeChanged();

    void beaconSent();

public slots:

private:
    BleAdvertisement *m_bleadvert;
    BleAdvertisementManager *m_manager;
    bool m_registered;
};

#endif // CONTROLLER_H
