#include <QtTest>

#include "contrac.h"
#include "contactstorage.h"
#include "bloomfilter.h"

#include "test_storage.h"

Test_Storage::Test_Storage(QObject *parent) : QObject(parent)
{

}

void Test_Storage::cleanup()
{
}

void Test_Storage::testDtk()
{
    QByteArray const tracing_key_base64("3UmKrtcQ2tfLE8UPSXHb4PtgRfE0E2xdSs+PGVIS8cc=");
    QByteArray tk_base64;
    QByteArray dtk_base64;
    QDateTime time;

    // Generate some keys, check the results
    Contrac * contrac;

    contrac = new Contrac();
    QVERIFY(contrac != NULL);

    contrac->setTk(QByteArray::fromBase64(tracing_key_base64));

    tk_base64 = contrac->tk();
    QCOMPARE(static_cast<unsigned int>(tk_base64.size()), TK_SIZE);
    tk_base64 = tk_base64.toBase64();
    QCOMPARE(tracing_key_base64, tk_base64);

    // Check the Daily Tracing Keys are generated correctly
    // Should use the standard test vectors when they're available

    time = QDateTime::fromMSecsSinceEpoch(0).addDays(12u);
    contrac->setTime(time);
    QCOMPARE(contrac->dayNumber(), 12u);
    dtk_base64 = contrac->dtk();
    QCOMPARE(static_cast<unsigned int>(dtk_base64.length()), DTK_SIZE);
    dtk_base64 = dtk_base64.toBase64();
    QCOMPARE(dtk_base64, QByteArray("AzZ389DsGecAjZqby1sLNQ=="));

    time = QDateTime::fromMSecsSinceEpoch(0).addDays(0u);
    contrac->setTime(time);
    QCOMPARE(contrac->dayNumber(), 0u);
    dtk_base64 = contrac->dtk();
    QCOMPARE(static_cast<unsigned int>(dtk_base64.length()), DTK_SIZE);
    dtk_base64 = dtk_base64.toBase64();
    QCOMPARE(dtk_base64, QByteArray("p7LrsTReTw3k721eIWDjRw=="));

    time = QDateTime::fromMSecsSinceEpoch(0).addDays(143u);
    contrac->setTime(time);
    QCOMPARE(contrac->dayNumber(), 143u);
    dtk_base64 = contrac->dtk();
    QCOMPARE(static_cast<unsigned int>(dtk_base64.length()), DTK_SIZE);
    dtk_base64 = dtk_base64.toBase64();
    QCOMPARE(dtk_base64, QByteArray("f6RZL/2wGCzxSBzZc9xVNQ=="));

    // Clean up
    delete contrac;
}

void Test_Storage::testRpi()
{
    QByteArrayList tracing_key_base64 = {
        "3UmKrtcQ2tfLE8UPSXHb4PtgRfE0E2xdSs+PGVIS8cc=",
        "U3CgpSjF0qFW8DNSTHVWF99few5FOW7RV7kA9j6LFTc=",
        };
    QByteArray tk;
    QByteArray rpi_base64;
    QDateTime dayStart;
    QDateTime time;

    // Generate some keys, check the results
    Contrac * contrac;

    contrac = new Contrac();
    QVERIFY(contrac != NULL);

    tk = QByteArray::fromBase64(tracing_key_base64[0]);
    QCOMPARE(static_cast<unsigned int>(tk.length()), TK_SIZE);
    contrac->setTk(tk);

    dayStart = QDateTime::fromMSecsSinceEpoch(0).addDays(9);
    contrac->setTime(dayStart);
    QCOMPARE(contrac->dayNumber(), static_cast<quint32>(9));

    // Check the Rolling Proximity Identifiers are generated correctly
    // Should use the standard test vectors when they're available

    time = dayStart.addSecs(10 * 60 * 0);
    contrac->setTime(time);
    QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(0));
    rpi_base64 = contrac->rpi();
    QCOMPARE(static_cast<unsigned int>(rpi_base64.length()), RPI_SIZE);
    rpi_base64 = rpi_base64.toBase64();
    QCOMPARE(rpi_base64, QByteArray("yStiu899O+6xvdLiUdrpsA=="));

    time = dayStart.addSecs(10 * 60 * 82);
    contrac->setTime(time);
    QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(82));
    rpi_base64 = contrac->rpi();
    QCOMPARE(static_cast<unsigned int>(rpi_base64.length()), RPI_SIZE);
    rpi_base64 = rpi_base64.toBase64();
    QCOMPARE(rpi_base64, QByteArray("aFTYIeEUGYKELi8TUUql+Q=="));

    time = dayStart.addSecs(10 * 60 * 143);
    contrac->setTime(time);
    QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(143));
    rpi_base64 = contrac->rpi();
    QCOMPARE(static_cast<unsigned int>(rpi_base64.length()), RPI_SIZE);
    rpi_base64 = rpi_base64.toBase64();
    QCOMPARE(rpi_base64, QByteArray("MK9mDdgTsgsh6Vxp0XhasA=="));

    tk = QByteArray::fromBase64(tracing_key_base64[1]);
    QCOMPARE(static_cast<unsigned int>(tk.length()), TK_SIZE);
    contrac->setTk(tk);

    dayStart = QDateTime::fromMSecsSinceEpoch(0).addDays(500);
    contrac->setTime(dayStart);
    QCOMPARE(contrac->dayNumber(), static_cast<quint32>(500));

    time = dayStart.addSecs(10 * 60 * 1);
    contrac->setTime(time);
    QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(1));
    rpi_base64 = contrac->rpi();
    QCOMPARE(static_cast<unsigned int>(rpi_base64.length()), RPI_SIZE);
    rpi_base64 = rpi_base64.toBase64();
    QCOMPARE(rpi_base64, QByteArray("layNswS/rQxovfYnBhjXJg=="));

    time = dayStart.addSecs(10 * 60 * 27);
    contrac->setTime(time);
    QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(27));
    rpi_base64 = contrac->rpi();
    QCOMPARE(static_cast<unsigned int>(rpi_base64.length()), RPI_SIZE);
    rpi_base64 = rpi_base64.toBase64();
    QCOMPARE(rpi_base64, QByteArray("BJyzAC3hyVfcWOhQ9paEFA=="));

    time = dayStart.addSecs(10 * 60 * 143);
    contrac->setTime(time);
    QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(143));
    rpi_base64 = contrac->rpi();
    QCOMPARE(static_cast<unsigned int>(rpi_base64.length()), RPI_SIZE);
    rpi_base64 = rpi_base64.toBase64();
    QCOMPARE(rpi_base64, QByteArray("ItETUQc372fOzJKHvF8z6w=="));

    // Clean up
    delete contrac;
}

void Test_Storage::testBloomFilter()
{
    BloomFilter bloomfilter(100, 32768, 11);
    Contrac contrac;
    int count;

    QDateTime date = QDateTime::fromMSecsSinceEpoch(qint64(1589228731) * qint64(1000));

    QByteArrayList included;
    QByteArrayList excluded;

    for (int pos = 0; pos < 2048; ++pos) {
        date = date.addSecs(60 * 10);
        contrac.setTime(date);
        QByteArray rpi = contrac.rpi();
        included.append(rpi);
        bloomfilter.add(rpi);
    }

    for (int pos = 0; pos < 2048; ++pos) {
        date = date.addSecs(60 * 10);
        contrac.setTime(date);
        QByteArray rpi = contrac.rpi();
        excluded.append(rpi);
    }

    count = 0;
    for (int pos = 0; pos < 2048; ++pos) {
        if (bloomfilter.test(included.at(pos))) {
            count++;
        }
    }
    QVERIFY(count == 2048);

    count = 0;
    for (int pos = 0; pos < 2048; ++pos) {
        if (bloomfilter.test(excluded.at(pos))) {
            count++;
        }
    }
    // This is stochastic, so could technically be any value up to 2048
    // If it gets too high, then the defaults for our Bloom Filter are probably wrong
    QVERIFY(count < 10);
}

void Test_Storage::testStorage()
{

}

void Test_Storage::testMatch()
{

}

QTEST_APPLESS_MAIN(Test_Storage)
