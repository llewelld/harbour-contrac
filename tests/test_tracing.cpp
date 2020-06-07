#include <QtTest>

#include "../contracd/src/contrac.h"
#include "../contracd/src/contactstorage.h"
#include "../contracd/src/bloomfilter.h"
#include "../contracd/src/exposurenotification.h"
#include "../contracd/src/exposurenotification_p.h"
#include "../contracd/src/exposureconfiguration.h"
#include "../contracd/src/contrac.pb.h"

#include "test_tracing.h"

namespace { // Anonymous namespace

quint32 countBloomFiles()
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    qDebug() << "Counting Bloom filter files in " << folder;
    QDir dataDir;
    QFileInfoList files;
    quint32 count;

    count = 0;
    dataDir.setPath(folder);
    dataDir.setFilter(QDir::Files);
    files = dataDir.entryInfoList();
    for (QFileInfo file : files) {
        QString extension = file.completeSuffix();
        if (extension == "bloom") {
            ++count;
        }
    }
    qDebug() << "Found " << count << " files";
    return count;
}

quint32 countDataFiles()
{
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/contacts";
    qDebug() << "Counting data files in " << folder;
    QDir dataDir;
    QFileInfoList files;
    quint32 count;

    count = 0;
    dataDir.setPath(folder);
    dataDir.setFilter(QDir::Files);
    files = dataDir.entryInfoList();
    for (QFileInfo file : files) {
        QString extension = file.completeSuffix();
        if (extension == "dat") {
            ++count;
        }
    }
    qDebug() << "Found " << count << " files";
    return count;
}

} // Anonymous namespace


Test_Tracing::Test_Tracing(QObject *parent) : QObject(parent)
{
    QCoreApplication::setOrganizationDomain("www.flypig.co.uk");
    QCoreApplication::setApplicationName("harbour-contrac-tests");
}

void Test_Tracing::cleanup()
{
    ContactStorage::clearAllDataFiles();
}

void Test_Tracing::testDtk()
{
    QByteArray const tracing_key_base64("3UmKrtcQ2tfLE8UPSXHb4PtgRfE0E2xdSs+PGVIS8cc=");
    QByteArray tk_base64;
    QByteArray dtk_base64;
    QDateTime time;

    // Generate some keys, check the results
    Contrac * contrac;

    contrac = new Contrac(this);
    QVERIFY(contrac != nullptr);

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

void Test_Tracing::testRpi()
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

    contrac = new Contrac(this);
    QVERIFY(contrac != nullptr);

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

void Test_Tracing::testBloomFilter()
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

void Test_Tracing::testStorage()
{
    quint32 day;
    QByteArray rpi_bytes;
    QDateTime time;

    ContactStorage::clearAllDataFiles();
    QCOMPARE(countDataFiles(), 0u);
    QCOMPARE(countBloomFiles(), 0u);

    Contrac * contrac;
    ContactStorage * storage;

    contrac = new Contrac(this);
    QVERIFY(contrac != nullptr);

    time = QDateTime::fromMSecsSinceEpoch(0).addDays(176).addSecs(60 * 10 * 5);
    contrac->setTime(time);

    storage = new ContactStorage(contrac);
    QVERIFY(storage != nullptr);

    // First ten files
    for (day = 176; day < 176 + 10; ++day) {
        time = QDateTime::fromMSecsSinceEpoch(0).addDays(day).addSecs(60 * 10 * 5);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(day));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(5));

        rpi_bytes = contrac->rpi();
        storage->addContact(5, rpi_bytes, -64);
    }

    delete storage;

    // All files should be there
    QCOMPARE(countDataFiles(), 10u);
    QCOMPARE(countBloomFiles(), 10u);

    storage = new ContactStorage(contrac);
    QVERIFY(storage != nullptr);

    // Next 8 files
    for (day = 176 + 11; day < 176 + 18; ++day) {
        time = QDateTime::fromMSecsSinceEpoch(0).addDays(day).addSecs(60 * 10 * 5);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(day));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(5));

        rpi_bytes = contrac->rpi();
        storage->addContact(5, rpi_bytes, -64);
    }

    delete storage;

    // Files will be harvested down to the last DAYS_TO_STORE (which should be at least 14)
    QVERIFY(DAYS_TO_STORE >= 14u);
    QCOMPARE(countDataFiles(), DAYS_TO_STORE);
    QCOMPARE(countBloomFiles(), DAYS_TO_STORE);

    storage = new ContactStorage(contrac);
    QVERIFY(storage != nullptr);

    storage->harvestOldData();

    QCOMPARE(countDataFiles(), DAYS_TO_STORE);
    QCOMPARE(countBloomFiles(), DAYS_TO_STORE);

    // Clean up
    delete storage;
    delete contrac;

    ContactStorage::clearAllDataFiles();
}

void Test_Tracing::testMatch()
{
    QByteArray const tracing_key_base64("3UmKrtcQ2tfLE8UPSXHb4PtgRfE0E2xdSs+PGVIS8cc=");
    QList<QPair<QByteArray, quint8>> beacon_list;
    QList<QPair<QByteArray, quint32>> diagnosis_list;
    // There are four matches in amongst this lot
    // (day, time) = (12, 15), (1175, 142), (1175, 67), (12, 93)
    quint32 beacon_days[9] = {1174 - DAYS_TO_STORE, 1171, 1170, 1172, 1173, 1175, 1175, 1174, 1170};
    quint8 beacon_times[9] = {33, 1, 15, 5, 101, 142, 67, 51, 93};
    // Note that the first diagnosis day will be dropped as out of range
    quint32 diagnosis_days[3] = {1174 - DAYS_TO_STORE, 1170, 1175};
    // Summarise the four matches
    QList<quint32> match_days({1170, 1175, 1175, 1170});
    QList<quint8> match_times({15, 142, 67, 93});
    int pos;
    QByteArray rpi_bytes;
    QByteArray dtk_bytes;
    QList<ContactMatch> matches;

    ContactStorage::clearAllDataFiles();

    // Generate some keys, check the results
    Contrac * contrac;
    ContactStorage * storage;

    contrac = new Contrac(this);
    QVERIFY(contrac != nullptr);

    storage = new ContactStorage(contrac);
    QVERIFY(storage != nullptr);

    contrac->setTk(QByteArray::fromBase64(tracing_key_base64));

    // Generate some beacons (as if collected over BlueTooth)
    for (pos = 0; pos < 9; ++pos) {
        QDateTime time = QDateTime::fromMSecsSinceEpoch(0).addDays(beacon_days[pos]).addSecs(60 * 10 * beacon_times[pos]);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(beacon_days[pos]));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(beacon_times[pos]));

        rpi_bytes = contrac->rpi();
        beacon_list.append(QPair<QByteArray, quint8>(rpi_bytes, beacon_times[pos]));

        ctinterval interval = intervalToCtInterval(beacon_times[pos]);
        storage->addContact(interval, rpi_bytes, -64);
    }

    // Generate some diagnosis data (as if provided by a diagnosis server)
    for (pos = 0; pos < 3; ++pos) {
        QDateTime time = QDateTime::fromMSecsSinceEpoch(0).addDays(diagnosis_days[pos]).addSecs(60 * 10 * 50);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(diagnosis_days[pos]));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(50));

        dtk_bytes = contrac->dtk();
        diagnosis_list.append(QPair<QByteArray, quint32>(dtk_bytes, diagnosis_days[pos]));
    }

    // Check that the matching algorithm identifies the beacons that match
    for (QPair<QByteArray, quint32> diagnosis : diagnosis_list) {
        QList<DiagnosisKey> dtks;
        dtks.append(DiagnosisKey(diagnosis.first, 0, 50, 1));
        matches = storage->findDtkMatches(diagnosis.second, dtks);
        QCOMPARE((matches.length() > 0), match_days.contains(diagnosis.second));
    }

    // Clean up
    delete storage;
    delete contrac;

    ContactStorage::clearAllDataFiles();
}

void Test_Tracing::testDiagnosis()
{
    bool result;

    ExposureNotification notification(this);
    ExposureConfiguration config;
    diagnosis::TemporaryExposureKeyExport keyExport;
    result = ExposureNotificationPrivate::loadDiagnosisKeys("/usr/share/harbour-contrac-tests/sample_diagnosis_key_file.zip", &keyExport);
    QVERIFY(result);
}

QTEST_APPLESS_MAIN(Test_Tracing)
