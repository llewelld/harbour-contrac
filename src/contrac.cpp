/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

// Needed for TK
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "hkdfsha256.h"

// Needed for DTK
//#include <openssl/kdf.h>
#include <openssl/evp.h>

// Needed for RPI
#include <openssl/hmac.h>

#include <QDebug>

#define DTK_INFO_PREFIX "CT-DTK"
#define RPI_INFO_PREFIX "CT-RPI"

#include "contrac.h"

Contrac::Contrac(QObject *parent) : QObject(parent)
  , m_day_number(0)
  , m_time_interval_number(0)
{
    m_time = QDateTime::currentDateTime();
    generateTracingKey();
}

bool Contrac::generateTracingKey()
{
    int result;
    unsigned char data[TK_SIZE];

    // tk <- CRNG(32)
    result = RAND_bytes(data, TK_SIZE);

    if (result == 1) {
        m_tk.clear();
        m_tk.append((char *)data, TK_SIZE);
    }
    else {
        qDebug() << "Error generating tracing key: " << ERR_get_error();
    }

    return (result == 1);
}

bool Contrac::generateDailyTracingKey(quint8 day_number)
{
    int result = 1;
    unsigned char encode[sizeof(DTK_INFO_PREFIX) + sizeof(day_number)];
    size_t out_length = 0;
    //EVP_PKEY_CTX *pctx = NULL;
    unsigned char const * tk;
    unsigned char data[DTK_SIZE];
    unsigned char salt[4];

    // dtk_i <- HKDF(tk, NULL, (UTF8("CT-DTK") || D_i), 16)

    if (result > 0) {
        // Produce Info sequence UTF8("CT-DTK") || D_i)
        // From the spec it's not clear whether this is string or byte concatenation.
        // Here we use byte, but it might have to be changed
        memcpy(encode, DTK_INFO_PREFIX, sizeof(DTK_INFO_PREFIX));
        ((uint32_t *)(encode + sizeof(DTK_INFO_PREFIX)))[0] = day_number;

        tk = (unsigned char *)m_tk.data();

        out_length = DTK_SIZE;
        result = HKDF(data, out_length, EVP_sha256(), tk, m_tk.size(), salt, 0, encode, sizeof(encode));
    }

    if ((result > 0) && (out_length == DTK_SIZE)) {
        m_dtk.clear();
        m_dtk.append((char *)data, DTK_SIZE);
        m_day_number = day_number;
        result = 1;
        emit dtkChanged();
    }

    if (result <= 0) {
        qDebug() << "Error generating daily key: " << ERR_get_error();
    }

    return (result > 0);
}

bool Contrac::generateRandomProximityIdentifier(quint8 time_interval_number)
{
    int result = 1;
    unsigned char encode[sizeof(RPI_INFO_PREFIX) + sizeof(time_interval_number)];
    unsigned char output[EVP_MAX_MD_SIZE];
    unsigned int out_length = 0;
    unsigned int pos;
    unsigned int min;
    unsigned char const * daily_key;
    unsigned char data[DTK_SIZE];

    // RPI_{i, j} <- Truncate(HMAC(dkt_i, (UTF8("CT-RPI") || TIN_j)), 16)

    if (result > 0) {
        // Produce Info sequence UTF8("CT-DTK") || D_i)
        // From the spec it's not clear whether this is string or byte concatenation.
        // Here we use byte, but it might have to be changed
        memcpy(encode, RPI_INFO_PREFIX, sizeof(RPI_INFO_PREFIX));
        ((uint8_t *)(encode + sizeof(RPI_INFO_PREFIX)))[0] = time_interval_number;
        out_length = sizeof(output);

        daily_key = (unsigned char *)m_dtk.data();
        HMAC(EVP_sha256(), daily_key, DTK_SIZE, encode, sizeof(encode), output, &out_length);

        //_Static_assert ((EVP_MAX_MD_SIZE >= 16), "HMAC buffer size too small");

        // Truncate and copy the result
        min = qMin(out_length, 16u);
        for (pos = 0; pos < min; ++pos) {
            data[pos] = output[pos];
        }
        // Zero out padding if there is any
        for (pos = min; pos < 16; ++pos) {
            data[pos] = 0;
        }
    }

    if (result > 0) {
        //data->time_interval_number = time_interval_number;
        m_rpi.clear();
        m_rpi.append((char *)data, RPI_SIZE);
        m_time_interval_number = time_interval_number;
        emit rpiChanged();
    }

    if (result <= 0) {
        qDebug() << "Error generating rolling proximity id: " << ERR_get_error();
    }

    return (result > 0);
}


QByteArray Contrac::tk() const
{
    return m_tk;
}

QByteArray Contrac::dtk() const
{
    return m_dtk;
}

QByteArray Contrac::rpi() const
{
    return m_rpi;
}

QDateTime Contrac::time() const
{
    return m_time;
}

void Contrac::setTk(const QByteArray &tk)
{
    if (m_tk != tk) {
        m_tk = tk;
        emit tkChanged();
    }
}

void Contrac::setTime(const QDateTime &time)
{
    bool result;
    quint64 epoch_seconds;

    if (m_time != time) {
        m_time = time;

        epoch_seconds = time.toMSecsSinceEpoch() / 1000;
        quint32 day_number = epochToDayNumber(epoch_seconds);
        if (m_day_number != day_number) {
            m_day_number = day_number;

            result = generateDailyTracingKey(m_day_number);
            if (!result) {
                qDebug() << "Daily Tracing Key generation failed";
            }
        }

        quint8 time_interval_number = epochToTimeIntervalNumber(epoch_seconds);
        if (m_time_interval_number != time_interval_number) {
            m_time_interval_number = time_interval_number;

            result = generateRandomProximityIdentifier(m_time_interval_number);
            if (!result) {
                qDebug() << "Random Proximity Identifier generation failed";
            }
        }

        emit timeChanged();
    }
}

void Contrac::updateKeys()
{
    //QDateTime now = QDateTime::currentDateTime();
    QDateTime time = m_time.addSecs(1);

    setTime(time);
}

quint32 Contrac::epochToDayNumber(quint64 epoch)
{
        quint32 day_number;

        // DayNumber <- (Number of Seconds since Epoch) / (60 * 60 * 24)
        day_number = epoch / (60 * 60 * 24);

        return day_number;
}

quint8 Contrac::epochToTimeIntervalNumber(quint64 epoch)
{
    quint8 time_interval_number;
    quint32 day_number;
    quint32 seconds;

    // TimeNumberInterval <- (Seconds Since Start of DayNumber) / (60 * 10)
    day_number = epochToDayNumber(epoch);
    seconds = epoch - (day_number * (60 * 60 * 24));

    time_interval_number = seconds / (60 * 10);

    // Falls in interval [0,143]
    if (time_interval_number > 143) {
        time_interval_number = 143;
    }

    return time_interval_number;
}

quint32 Contrac::dayNumber() const
{
    return m_day_number;
}

quint8 Contrac::timeIntervalNumber () const
{
    return m_time_interval_number;
}
