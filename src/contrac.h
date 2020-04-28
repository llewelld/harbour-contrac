/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#ifndef CONTRAC_H
#define CONTRAC_H

#include <QObject>
#include <QDateTime>

#define TK_SIZE (32)
#define DTK_SIZE (16)
#define RPI_SIZE (16)

class Contrac : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray tk READ tk WRITE setTk NOTIFY tkChanged)
    Q_PROPERTY(QByteArray dtk READ dtk NOTIFY dtkChanged)
    Q_PROPERTY(QByteArray rpi READ rpi NOTIFY rpiChanged)

    Q_PROPERTY(QDateTime time READ time WRITE setTime NOTIFY timeChanged)

    Q_PROPERTY(quint32 dayNumber READ dayNumber NOTIFY timeChanged)
    Q_PROPERTY(quint8 timeIntervalNumber READ timeIntervalNumber NOTIFY timeChanged)

public:
    explicit Contrac(QObject *parent = nullptr);

    QByteArray tk() const;
    QByteArray dtk() const;
    QByteArray rpi() const;
    QDateTime time() const;
    quint32 dayNumber() const;
    quint8 timeIntervalNumber () const;

    void setTk(const QByteArray &tk);
    void setTime(const QDateTime &time);

signals:
    void tkChanged();
    void dtkChanged();
    void rpiChanged();
    void timeChanged();

public slots:
    void updateKeys();

private:
    bool generateTracingKey();
    bool generateDailyTracingKey(quint8 day_number);
    bool generateRandomProximityIdentifier(quint8 time_interval_number);

    static quint32 epochToDayNumber(quint64 epoch);
    static quint8 epochToTimeIntervalNumber(quint64 epoch);

private:
    QByteArray m_tk;
    QByteArray m_dtk;
    QByteArray m_rpi;
    QDateTime m_time;
    quint32 m_day_number;
    quint8 m_time_interval_number;
};

#endif // CONTRAC_H
