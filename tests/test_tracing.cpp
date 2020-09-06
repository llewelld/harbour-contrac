#include <QtTest>

#include "../contracd/src/contrac.h"
#include "../contracd/src/contactstorage.h"
#include "../contracd/src/bloomfilter.h"
#include "../contracd/src/metadata.h"
#include "../contracd/src/settings.h"
#include "../contracd/src/exposurenotification.h"
#include "../contracd/src/exposurenotification_p.h"
#include "../contracd/src/exposureconfiguration.h"
#include "../contracd/proto/contrac.pb.h"

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
    QCoreApplication::setOrganizationName("harbour-contrac");
    QCoreApplication::setApplicationName("harbour-contrac-tests");
}

void Test_Tracing::init()
{
    Settings::instantiate(this);
}

void Test_Tracing::cleanup()
{
    ContactStorage::clearAllDataFiles();

    Settings &settings = Settings::getInstance();
    delete &settings;
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    qDebug() << "Removing folder" << folder;
    QDir dir(folder);
    dir.removeRecursively();
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

    time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(12u);
    contrac->setTime(time);
    QCOMPARE(contrac->dayNumber(), 12u);
    dtk_base64 = contrac->dtk();
    QCOMPARE(static_cast<unsigned int>(dtk_base64.length()), DTK_SIZE);
    dtk_base64 = dtk_base64.toBase64();
    QCOMPARE(dtk_base64, QByteArray("AzZ389DsGecAjZqby1sLNQ=="));

    time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(0u);
    contrac->setTime(time);
    QCOMPARE(contrac->dayNumber(), 0u);
    dtk_base64 = contrac->dtk();
    QCOMPARE(static_cast<unsigned int>(dtk_base64.length()), DTK_SIZE);
    dtk_base64 = dtk_base64.toBase64();
    QCOMPARE(dtk_base64, QByteArray("p7LrsTReTw3k721eIWDjRw=="));

    time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(143u);
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

    dayStart = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(9);
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

    dayStart = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(500);
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

void Test_Tracing::testMetadata()
{
    Contrac * contrac;
    QDateTime time;
    QByteArray dtk;
    QByteArray rpi;
    QByteArray content[2];
    QByteArray aem[2];
    QByteArray decrypted;
    Metadata metadata[2];

    contrac = new Contrac(this);
    time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(12u);
    contrac->setTime(time);
    QCOMPARE(contrac->dayNumber(), 12u);
    dtk = contrac->dtk();
    QCOMPARE(static_cast<unsigned int>(dtk.length()), DTK_SIZE);

    rpi = contrac->rpi();
    metadata[0].setDtk(dtk);
    metadata[0].setRpi(rpi);
    metadata[0].setTxPower(-35);

    content[0] = metadata[0].metadata();
    QCOMPARE(content[0].size(), 4);
    QCOMPARE(content[0].at(0), char(0b01000000));
    QCOMPARE(static_cast<qint8>(content[0].at(1)), qint8(-35));
    QCOMPARE(content[0].at(2), char(0));
    QCOMPARE(content[0].at(3), char(0));

    aem[0] = metadata[0].encryptedMetadata();

    QCOMPARE(static_cast<unsigned int>(aem[0].length()), AEM_SIZE);
    QVERIFY(aem[0] != content);

    metadata[1].setDtk(dtk);
    metadata[1].setRpi(rpi);
    metadata[1].setEncryptedMetadata(aem[0]);

    decrypted = metadata[1].metadata();
    QCOMPARE(decrypted.size(), 4);
    QCOMPARE(content[0], decrypted);

    time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(165u);
    contrac->setTime(time);
    QCOMPARE(contrac->dayNumber(), 165u);
    dtk = contrac->dtk();
    QCOMPARE(static_cast<unsigned int>(dtk.length()), DTK_SIZE);

    rpi = contrac->rpi();
    metadata[0].setDtk(dtk);
    metadata[0].setRpi(rpi);
    metadata[0].setTxPower(127);

    content[1] = metadata[0].metadata();

    QVERIFY(content[0] != content[1]);

    QCOMPARE(content[1].size(), 4);
    QCOMPARE(content[1].at(0), char(0b01000000));
    QCOMPARE(static_cast<qint8>(content[1].at(1)), qint8(127));
    QCOMPARE(content[1].at(2), char(0));
    QCOMPARE(content[1].at(3), char(0));

    aem[1] = metadata[0].encryptedMetadata();

    QVERIFY(aem[0] != aem[1]);

    QCOMPARE(static_cast<unsigned int>(aem[1].length()), AEM_SIZE);
    QVERIFY(aem[1] != content);

    metadata[1].setDtk(dtk);
    metadata[1].setRpi(rpi);
    metadata[1].setEncryptedMetadata(aem[1]);

    decrypted = metadata[1].metadata();
    QCOMPARE(decrypted.size(), 4);
    QCOMPARE(content[1], decrypted);

    // Clean up
    delete contrac;
}

void Test_Tracing::testMatchAggregation()
{
    QByteArray const tracing_key_base64("p4QrDr0/C1D5ejQDN9tWnZdhRiKijGc9VeBlro8JfIQ=");
    QList<QPair<QByteArray, quint32>> diagnosis_list;
    // There are six matches in amongst this lot
    // (day, time) = (1170, 4500), (1175, 20100), (1175, 42600), (1170, 27850), (1170, 27500), (1170, 27600)
    quint32 beacon_days[11] = {1174 - DAYS_TO_STORE, 1171, 1170, 1172, 1173, 1175, 1175, 1174, 1170, 1170, 1170};
    // These beacon_times are measured in 2 second intervals (ctinterval)
    ctinterval beacon_times[11] = {9900, 300, 4500, 1500, 30300, 20100, 42600, 15300, 27850, 27500, 27600};
    qint8 beacon_invalidate[11] = {-1, -1, -1, -1, -1, 2, 3, -1, -1, -1, -1};
    qint8 beacon_txPower[11] = {-31, -31, -31, -100, -31, -100, -31, -31, -31, -31, -31};
    qint8 beacon_rssi[11] = {-64, -64, -64, -64, -64, -64, -64, -64, -38, -64, -127};
    // Note that the first diagnosis day will be dropped as out of range
    quint32 diagnosis_days[3] = {1174 - DAYS_TO_STORE, 1170, 1175};
    QMap<quint32, QList<ctinterval>> expectedMatches;
    int pos;
    QByteArray rpi_bytes;
    QByteArray dtk_bytes;
    QList<ContactMatch> matches;
    Metadata metadata;
    QByteArray aem;
    QList<ExposureInformation> exposureInfoList;
    ExposureConfiguration configuration;

    ContactStorage::clearAllDataFiles();

    // Set up the configuration
    configuration.setMinimumRiskScore(0);
    configuration.setAttenuationScores(QList<qint32>({0, 1, 2, 3, 4, 5, 6, 7}));
    configuration.setDaysSinceLastExposureScores(QList<qint32>({0, 1, 2, 3, 4, 5, 6, 7}));
    configuration.setDurationScores(QList<qint32>({0, 1, 2, 3, 4, 5, 6, 7}));
    configuration.setTransmissionRiskScores(QList<qint32>({0, 1, 2, 3, 4, 5, 6, 7}));
    configuration.setAttenuationWeight(1.0);
    configuration.setDaysSinceLastExposureWeight(1.0);
    configuration.setDurationWeight(1.0);
    configuration.setTransmissionRiskWeight(1.0);
    configuration.setDurationAtAttenuationThresholds(QList<qint32>({8, 64}));

    // Generate some keys, check the results
    Contrac * contrac;
    ContactStorage * storage;

    contrac = new Contrac(this);
    QVERIFY(contrac != nullptr);

    storage = new ContactStorage(contrac);
    QVERIFY(storage != nullptr);

    contrac->setTk(QByteArray::fromBase64(tracing_key_base64));

    // Generate some beacons (as if collected over BlueTooth)
    for (pos = 0; pos < 11; ++pos) {
        QDateTime time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(beacon_days[pos]).addSecs((CTINTERVAL_DURATION / 1000) * beacon_times[pos]);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(beacon_days[pos]));
        QCOMPARE(contrac->timeIntervalNumber(), ctIntervalToInterval(beacon_times[pos]));

        dtk_bytes = contrac->dtk();
        rpi_bytes = contrac->rpi();

        metadata.setDtk(dtk_bytes);
        metadata.setRpi(rpi_bytes);
        metadata.setTxPower(beacon_txPower[pos]);
        aem = metadata.encryptedMetadata();
        QVERIFY(metadata.valid());
        if ((beacon_invalidate[pos] >= 0) && (beacon_invalidate[pos] < int(AEM_SIZE))) {
            aem[beacon_invalidate[pos]] = aem.at(beacon_invalidate[pos]) ^ char(0b00000001);
        }

        storage->addContact(beacon_times[pos], rpi_bytes, aem, beacon_rssi[pos]);
    }

    // Generate some diagnosis data (as if provided by a diagnosis server)
    for (pos = 0; pos < 3; ++pos) {
        QDateTime time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(diagnosis_days[pos]).addSecs(60 * 10 * 50);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(diagnosis_days[pos]));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(50));

        dtk_bytes = contrac->dtk();
        diagnosis_list.append(QPair<QByteArray, quint32>(dtk_bytes, diagnosis_days[pos]));
    }

    // Verify the results; they should look like this
    // Exposure Info, day: 1170
    //    dateMillisSinceEpoch: 2303752192
    //    durationMinutes: 5
    //    attenuationValue: 28
    //    transmissionRiskLevel: 4
    //    totalRiskScore: 112
    //    attenuationDurations: (0, 1, 0)
    //
    //    dateMillisSinceEpoch: 2303752192
    //    durationMinutes: 15
    //    attenuationValue: 29
    //    transmissionRiskLevel: 4
    //    totalRiskScore: 336
    //    attenuationDurations: (9, 1, 4)
    //
    // Exposure Info, day: 1175
    // Note these should be empty due to failed validation
    //    dateMillisSinceEpoch: 2735752192
    //    durationMinutes: 0
    //    attenuationValue: 0
    //    transmissionRiskLevel: 4
    //    totalRiskScore: 0
    //    attenuationDurations: (0, 0, 0)
    //
    //    dateMillisSinceEpoch: 2735752192
    //    durationMinutes: 0
    //    attenuationValue: 0
    //    transmissionRiskLevel: 4
    //    totalRiskScore: 0
    //    attenuationDurations: (0, 0, 0)

    // Set up the expected matches
    expectedMatches.insert(1170, {4500, 27850, 27500, 27600});
    expectedMatches.insert(1175, {20100, 42600});
    struct AggregateMatch {
        quint64 dateMillisSinceEpoch;
        qint32 durationMinutes;
        qint32 attenuationValue;
        qint32 transmissionRiskLevel;
        qint32 totalRiskScore;
        QList<qint32> attenuationDurations;
    };
    QList<AggregateMatch> aggregateMatches;
    aggregateMatches += {2303752192, 5, 28, 4, 112, {0, 1, 0}};
    aggregateMatches += {2303752192, 15, 29, 4, 336, {9, 1, 4}};
    aggregateMatches += {2735752192, 0, 0, 4, 0, {0, 0, 0}};
    aggregateMatches += {2735752192, 0, 0, 4, 0, {0, 0, 0}};

    int total_matches = 0;
    // Check that the matching algorithm identifies the beacons that match
    for (QPair<QByteArray, quint32> diagnosis : diagnosis_list) {
        QList<DiagnosisKey> dtks;
        dtks.append(DiagnosisKey(diagnosis.first, DiagnosisKey::RiskLevelMedium, diagnosis.second * 144, 144));
        matches = storage->findDtkMatches(diagnosis.second, dtks);
        QCOMPARE((matches.length() > 0), expectedMatches.contains(diagnosis.second));
        for (ContactMatch match : matches) {
            total_matches += match.m_rpis.length();

            QList<ctinterval> intervals = expectedMatches[diagnosis.second];
            for (RpiDataItem rpi : match.m_rpis) {
                QVERIFY(intervals.contains(rpi.m_interval));
                intervals.removeOne(rpi.m_interval);
                expectedMatches[diagnosis.second] = intervals;
            }
        }

        exposureInfoList = ExposureNotificationPrivate::aggregateExposureData(diagnosis.second, configuration, matches, 0);
        for (ExposureInformation exposureInfo : exposureInfoList) {
            bool match = false;
            int checkPos;
            for (checkPos = 0; !match && checkPos < aggregateMatches.length(); ++checkPos) {
                AggregateMatch check = aggregateMatches[checkPos];
                if ((check.dateMillisSinceEpoch == exposureInfo.dateMillisSinceEpoch()) && (check.durationMinutes == exposureInfo.durationMinutes()) && check.attenuationValue == exposureInfo.attenuationValue() && check.transmissionRiskLevel == exposureInfo.transmissionRiskLevel() && check.totalRiskScore == exposureInfo.totalRiskScore() && check.attenuationDurations == exposureInfo.attenuationDurations()) {
                    match = true;
                    aggregateMatches.removeAt(checkPos);
                }
            }
            QVERIFY(match);
        }
    }

    QCOMPARE(total_matches, 6);
    QCOMPARE(aggregateMatches.length(), 0);

    // Clean up
    delete storage;
    delete contrac;

    ContactStorage::clearAllDataFiles();
}

void Test_Tracing::testBloomFilter()
{
    BloomFilter bloomfilter(100, 32768, 11);
    Contrac contrac;
    int count;

    QDateTime date = QDateTime::fromMSecsSinceEpoch(qint64(1589228731) * qint64(1000), Qt::UTC);

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
    QByteArray dtk_bytes;
    QByteArray rpi_bytes;
    QDateTime time;
    Metadata metadata;
    QByteArray aem;

    ContactStorage::clearAllDataFiles();
    QCOMPARE(countDataFiles(), 0u);
    QCOMPARE(countBloomFiles(), 0u);

    Contrac * contrac;
    ContactStorage * storage;

    contrac = new Contrac(this);
    QVERIFY(contrac != nullptr);

    time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(176).addSecs(60 * 10 * 5);
    contrac->setTime(time);

    storage = new ContactStorage(contrac);
    QVERIFY(storage != nullptr);

    // First ten files
    for (day = 176; day < 176 + 10; ++day) {
        time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(day).addSecs(60 * 10 * 5);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(day));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(5));

        dtk_bytes = contrac->dtk();
        rpi_bytes = contrac->rpi();

        metadata.setDtk(dtk_bytes);
        metadata.setRpi(rpi_bytes);
        metadata.setTxPower(-25);
        aem = metadata.encryptedMetadata();
        QVERIFY(metadata.valid());

        storage->addContact(5, rpi_bytes, aem, -64);
    }

    delete storage;

    // All files should be there
    QCOMPARE(countDataFiles(), 10u);
    QCOMPARE(countBloomFiles(), 10u);

    storage = new ContactStorage(contrac);
    QVERIFY(storage != nullptr);

    // Next 8 files
    for (day = 176 + 11; day < 176 + 18; ++day) {
        time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(day).addSecs(60 * 10 * 5);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(day));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(5));

        dtk_bytes = contrac->dtk();
        rpi_bytes = contrac->rpi();

        metadata.setDtk(dtk_bytes);
        metadata.setRpi(rpi_bytes);
        metadata.setTxPower(-25);
        aem = metadata.encryptedMetadata();
        QVERIFY(metadata.valid());

        storage->addContact(5, rpi_bytes, aem, -64);
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
    QList<QPair<QByteArray, quint32>> diagnosis_list;
    // There are four matches in amongst this lot
    // (day, time) = (1170, 15), (1175, 142), (1175, 67), (1170, 93)
    quint32 beacon_days[9] = {1174 - DAYS_TO_STORE, 1171, 1170, 1172, 1173, 1175, 1175, 1174, 1170};
    quint8 beacon_times[9] = {33, 1, 15, 5, 101, 67, 142, 51, 93};
    // Note that the first diagnosis day will be dropped as out of range
    quint32 diagnosis_days[3] = {1174 - DAYS_TO_STORE, 1170, 1175};
    // Summarise the four matches
    QList<quint32> match_days({1170, 1175, 1175, 1170});
    QList<quint8> match_times({15, 67, 142, 93});
    int pos;
    QByteArray rpi_bytes;
    QByteArray dtk_bytes;
    QList<ContactMatch> matches;
    Metadata metadata;
    QByteArray aem;

    // The expected matches
    QMap<quint32, QList<quint8>> expectecMatches;
    expectecMatches.insert(1170, {15, 93});
    expectecMatches.insert(1175, {67, 142});

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
        QDateTime time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(beacon_days[pos]).addSecs(60 * 10 * beacon_times[pos]);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(beacon_days[pos]));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(beacon_times[pos]));

        dtk_bytes = contrac->dtk();
        rpi_bytes = contrac->rpi();

        metadata.setDtk(dtk_bytes);
        metadata.setRpi(rpi_bytes);
        metadata.setTxPower(-31);
        aem = metadata.encryptedMetadata();
        QVERIFY(metadata.valid());

        ctinterval interval = intervalToCtInterval(beacon_times[pos]);
        storage->addContact(interval, rpi_bytes, aem, -64);
    }

    // Generate some diagnosis data (as if provided by a diagnosis server)
    for (pos = 0; pos < 3; ++pos) {
        QDateTime time = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC).addDays(diagnosis_days[pos]).addSecs(60 * 10 * 50);
        contrac->setTime(time);
        QCOMPARE(contrac->dayNumber(), static_cast<quint32>(diagnosis_days[pos]));
        QCOMPARE(contrac->timeIntervalNumber(), static_cast<quint8>(50));

        dtk_bytes = contrac->dtk();
        diagnosis_list.append(QPair<QByteArray, quint32>(dtk_bytes, diagnosis_days[pos]));
    }

    // Check that the matching algorithm identifies the beacons that match
    int total_matches = 0;
    for (QPair<QByteArray, quint32> diagnosis : diagnosis_list) {
        QList<DiagnosisKey> dtks;
        dtks.append(DiagnosisKey(diagnosis.first, DiagnosisKey::RiskLevelMedium, diagnosis.second * 144, 144));
        matches = storage->findDtkMatches(diagnosis.second, dtks);

        for (ContactMatch match : matches) {
            QList<quint8> timeMatches = expectecMatches[diagnosis.second];
            for (RpiDataItem rpi : match.m_rpis) {
                ++total_matches;
                QVERIFY(timeMatches.contains(ctIntervalToInterval(rpi.m_interval)));
                if (timeMatches.contains(ctIntervalToInterval(rpi.m_interval))) {
                    timeMatches.removeOne(ctIntervalToInterval(rpi.m_interval));
                    expectecMatches[diagnosis.second] = timeMatches;
                }
            }
        }
    }
    QCOMPARE(total_matches, 4);

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
