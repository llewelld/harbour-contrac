#include <QList>
#include <QDebug>
#include <QRunnable>
#include <QThreadPool>
#include <QCoreApplication>
#include <string.h>

#include "temporaryexposurekey.h"
#include "contrac.pb.h"
#include "zipistreambuffer.h"
#include "contactinterval.h"
#include "exposuresummary.h"
#include "settings.h"
#include "providediagnostickeys.h"

#include "exposurenotification.h"
#include "exposurenotification_p.h"

#define MAX_DIAGNOSIS_KEYS (1024)
// The spaces are intentional, to pad to 16 bytes
#define EXPORT_BIN_HEADER QLatin1String("EK Export v1    ")

ExposureNotificationPrivate::ExposureNotificationPrivate(ExposureNotification *q)
    : QObject(q)
    , q_ptr(q)
    , m_status(ExposureNotification::Success)
{
    qDebug() << "Contrac";
    m_contrac = new Contrac(q);
    qDebug() << "Controller";
    m_controller = new Controller(q);
    qDebug() << "Scanner";
    m_scanner = new BleScanner(q);
    qDebug() << "Contacts";
    m_contacts = new ContactStorage(m_contrac);
    qDebug() << "Timer";
    // Update at least every ten minutes when active
    m_intervalUpdate.setInterval(5 * 1000);
    m_intervalUpdate.setSingleShot(false);

    qDebug() << "Load";
    QByteArray tk = Settings::getInstance().tracingKey();
    if (tk.isEmpty()) {
        qDebug() << "Generating new tracing key";
        m_contrac->generateTracingKey();
        tk = m_contrac->tk();
        Settings::getInstance().setTracingKey(tk);
    }
    else {
        qDebug() << "Using existing tracing key";
        m_contrac->setTk(tk);
    }
    m_contrac->updateKeys();

    qDebug() << "Connections";
    q_ptr->connect(m_contrac, &Contrac::rpiChanged, q_ptr, &ExposureNotification::onRpiChanged);
    q_ptr->connect(m_contrac, &Contrac::timeChanged, m_contacts, &ContactStorage::onTimeChanged);
    q_ptr->connect(&m_intervalUpdate, &QTimer::timeout, q_ptr, &ExposureNotification::intervalUpdate);
    q_ptr->connect(m_scanner, &BleScanner::beaconDiscovered, q_ptr, &ExposureNotification::beaconDiscovered);
    q_ptr->connect(m_scanner, &BleScanner::scanChanged, this, &ExposureNotificationPrivate::scanChanged);
    q_ptr->connect(m_scanner, &BleScanner::busyChanged, q_ptr, &ExposureNotification::isBusyChanged);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &ExposureNotificationPrivate::terminating, Qt::DirectConnection);
}

ExposureNotificationPrivate::~ExposureNotificationPrivate()
{
    q_ptr->disconnect(m_scanner, &BleScanner::beaconDiscovered, q_ptr, &ExposureNotification::beaconDiscovered);
    q_ptr->disconnect(&m_intervalUpdate, &QTimer::timeout, q_ptr, &ExposureNotification::intervalUpdate);
    q_ptr->disconnect(m_contrac, &Contrac::timeChanged, m_contacts, &ContactStorage::onTimeChanged);
    q_ptr->disconnect(m_contrac, &Contrac::rpiChanged, q_ptr, &ExposureNotification::onRpiChanged);
    q_ptr->disconnect(m_scanner, &BleScanner::scanChanged, this, &ExposureNotificationPrivate::scanChanged);
    q_ptr->disconnect(m_scanner, &BleScanner::busyChanged, q_ptr, &ExposureNotification::isBusyChanged);
}

ExposureNotification::ExposureNotification(QObject *parent)
    : QObject(parent)
    , d_ptr(new ExposureNotificationPrivate(this))
{
}

ExposureNotification::~ExposureNotification()
{
    Q_D(ExposureNotification);

    delete d;
}

bool ExposureNotification::isEnabled() const
{
    Q_D(const ExposureNotification);

    return d->m_scanner->scan();
}

bool ExposureNotification::isBusy() const
{
    Q_D(const ExposureNotification);

    return d->m_scanner->busy();
}

quint32 ExposureNotification::maxDiagnosisKeys() const
{
    return MAX_DIAGNOSIS_KEYS;
}

ExposureNotification::Status ExposureNotification::status() const
{
    Q_D(const ExposureNotification);

    return d->m_status;
}

void ExposureNotification::start()
{
    Q_D(ExposureNotification);

    d->m_contrac->updateKeys();

    if (!d->m_scanner->scan()) {
        d->m_scanner->setScan(true);
        d->m_controller->registerAdvert();
    }
    d->m_intervalUpdate.start();
}

void ExposureNotification::stop()
{
    Q_D(ExposureNotification);

    d->m_intervalUpdate.stop();
    if (d->m_scanner->scan()) {
        d->m_scanner->setScan(false);
        d->m_controller->unRegisterAdvert();
    }
}

// Return the last 14 days of history
QList<TemporaryExposureKey> ExposureNotification::getTemporaryExposureKeyHistory()
{
    Q_D(ExposureNotification);

    QList<TemporaryExposureKey> keys;
    quint32 today;
    quint32 day;

    today = d->m_contrac->dayNumber();
    day = today < 14 ? 0 : today - 14;
    while (day < today) {
        QByteArray dtk;
        dtk = d->m_contrac->dailyTracingKey(d->m_contrac->tk(), day);
        TemporaryExposureKey key(this);
        key.setKeyData(dtk);
        key.setRollingStartNumber(day * 144);
        key.setRollingPeriod(144);
        // TODO: Figure out where this is supposed to come from
        key.setTransmissionRiskLevel(TemporaryExposureKey::RiskLevelMedium);
        keys.append(key);
        ++day;
    }

    return keys;
}

void ExposureNotification::provideDiagnosisKeys(QVector<QString> const &keyFiles, ExposureConfiguration const &configuration, QString token)
{
    Q_D(ExposureNotification);

    d->m_contrac->updateKeys();
    qint8 rssiCorrection = Settings::getInstance().rssiCorrection();

    // This is a potentially very long-running process, so we execute it in a background thread
    // The actionExposureStateUpdated signal is sent out to the app when the process completes
    // The task is deleted automatically when the QRunnable completes
    ProvideDiagnosticKeys * task = new ProvideDiagnosticKeys(d, keyFiles, configuration, token, rssiCorrection);
    task->setAutoDelete(false);
    connect(task, &ProvideDiagnosticKeys::taskFinished, d, &ExposureNotificationPrivate::taskFinished);
    connect(task, &ProvideDiagnosticKeys::actionExposureStateUpdated, this, &ExposureNotification::actionExposureStateUpdated);
    connect(d, &ExposureNotificationPrivate::terminating, task, &ProvideDiagnosticKeys::requestTerminate, Qt::DirectConnection);
    d->m_runningTasks.insert(token, task);
    emit exposureStateChanged(token);

    QThreadPool::globalInstance()->start(task);
}

void ExposureNotificationPrivate::taskFinished(QString const token)
{
    Q_Q(ExposureNotification);

    ProvideDiagnosticKeys * task = m_runningTasks.value(token);
    m_runningTasks.remove(token);
    Q_ASSERT(task);
    // Update the newly processed data
    if (!task->shouldTerminate()) {
        qDebug() << "Diagnosis key processing completed successfully";
        m_lastProcessTime.insert(token, task->startTime());

        // Values will accumulate
        // We assume the same keyFiles won't be provided more than once
        QList<ExposureInformation> exposureInfoList = m_exposures.value(token);
        exposureInfoList.append(task->exposureInfoList());
        m_exposures.insert(token, exposureInfoList);
    }
    else {
        qDebug() << "Diagnosis key processing terminated before completion";
    }
    // Delete the task
    delete task;

    qDebug() << "Sending actionExposureStateUpdated signal";
    emit q->actionExposureStateUpdated(token);
    emit q->exposureStateChanged(token);
}

ExposureNotification::ExposureState ExposureNotification::exposureState(QString const &token) const
{
    Q_D(const ExposureNotification);

    ExposureState state;

    if (d->m_runningTasks.contains(token)) {
        state = Processing;
    }
    else {
        if (d->m_exposures.contains(token)) {
            state = Available;
        }
        else {
            state = None;
        }
    }

    return state;
}

bool ExposureNotificationPrivate::loadDiagnosisKeys(QString const &keyFile, diagnosis::TemporaryExposureKeyExport *keyExport)
{
    QuaZip quazip(keyFile);
    bool result = true;
    QStringList files;

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    result = (keyExport != nullptr);

    if (result) {
        result = quazip.open(QuaZip::mdUnzip, nullptr);
    }

    if (result) {
        // The zip archive must contain exactly two entries
        files = quazip.getFileNameList();
        if ((files.size() != 2) || !files.contains("export.sig") || !files.contains("export.bin")) {
            result = false;
        }
    }

    //TODO: Check signature file

    if (result) {
        ZipIStreamBuffer streambuf(quazip, "export.bin");
        if (!streambuf.success()) {
            qDebug() << "Zip stream creation failed";
        }
        std::istream stream(&streambuf);
        QByteArray header(16, '\0');
        stream.read(header.data(), 16);

        result = (header == QString(EXPORT_BIN_HEADER));

        if (result) {
            result = keyExport->ParseFromIstream(&stream);
        }
    }

    if (result) {
        qDebug() << "Diagnosis file: " << keyExport->keys().size() <<"keys;" << keyExport->region().data() << keyExport->start_timestamp() << "-" << keyExport->end_timestamp();
    }
    else {
        qDebug() << "TemporaryExposureKeyExport proto file failed to load";
    }

    return result;
}

void ExposureNotificationPrivate::scanChanged()
{
    Q_Q(ExposureNotification);

    Settings &settings = Settings::getInstance();
    bool scan = m_scanner->scan();
    settings.setEnabled(scan);

    emit q->isEnabledChanged();
}

ExposureSummary ExposureNotification::getExposureSummary(QString const &token) const
{
    Q_D(const ExposureNotification);

    QList<ExposureInformation> exposureInfoList;
    ExposureSummary summary;
    quint64 mostRecent;
    quint32 daysAgo;
    qint32 maxRiskScore;
    qint32 summationRiskScore;
    QList<qint32> attenuationDurations = QList<qint32>({0, 0, 0});

    if (d->m_exposures.contains(token)) {
        exposureInfoList = d->m_exposures.value(token);

        summationRiskScore = 0;
        mostRecent = 0;
        maxRiskScore = 0;
        for (ExposureInformation const &exposure : exposureInfoList) {
            if (exposure.dateMillisSinceEpoch() > mostRecent) {
                mostRecent = exposure.dateMillisSinceEpoch();
            }
            if (exposure.totalRiskScore() > maxRiskScore) {
                maxRiskScore = exposure.totalRiskScore();
            }
            attenuationDurations[0] += exposure.attenuationDurations()[0];
            attenuationDurations[1] += exposure.attenuationDurations()[1];
            attenuationDurations[2] += exposure.attenuationDurations()[2];

            summationRiskScore += exposure.totalRiskScore();
        }

        if (mostRecent == 0) {
            daysAgo = 0;
        }
        else {
            qint64 daysDelta = (QDateTime::currentMSecsSinceEpoch() - qint64(mostRecent)) / (24 * 60 * 60 * 1000);
            daysAgo = quint32(daysDelta);
        }

        summary.setDaysSinceLastExposure(daysAgo);
        summary.setMatchedKeyCount(static_cast<quint32>(exposureInfoList.count()));
        summary.setMaximumRiskScore(maxRiskScore);
        summary.setAttenuationDurations(attenuationDurations);
        summary.setSummationRiskScore(summationRiskScore);
    }

    return summary;
}

QList<ExposureInformation> ExposureNotification::getExposureInformation(QString const &token) const
{
    Q_D(const ExposureNotification);

    return d->m_exposures.value(token, QList<ExposureInformation>());
}

quint32 ExposureNotification::getMaxDiagnosisKeys() const
{
    return maxDiagnosisKeys();
}

void ExposureNotification::resetAllData()
{
    Q_D(ExposureNotification);

    d->m_contrac->updateKeys();
    d->m_contrac->generateTracingKey();
    d->m_contrac->saveTracingKey();
    d->m_contacts->clearAllDataFiles();
}

void ExposureNotification::beaconDiscovered(const QString &, const QByteArray &data, qint16 rssi)
{
    Q_D(ExposureNotification);
    QByteArray rpi;
    QByteArray aem;
    qDebug() << "Beacon discovered";

    if (data.length() == (RPI_SIZE + AEM_SIZE)) {
        rpi = data.mid(0, RPI_SIZE);
        qDebug() << "RPI:" << rpi.toHex();
        aem = data.mid(RPI_SIZE, AEM_SIZE);
        qDebug() << "AEM:" << aem.toHex();

        qint64 millisecondsSinceEpoch = QDateTime::currentDateTime().toMSecsSinceEpoch();

        ctinterval interval = millisecondsToCtInterval(millisecondsSinceEpoch);
        d->m_contrac->updateKeys();
        d->m_contacts->addContact(interval, rpi, aem, rssi);

        emit beaconReceived();
    }
    else {
        qDebug() << "Beacon has incorrect length: " << data.length();
    }
}

void ExposureNotification::intervalUpdate()
{
    Q_D(ExposureNotification);

    qDebug() << "intervalUpdate";
    d->m_contrac->updateKeys();
}

void ExposureNotification::onRpiChanged()
{
    Q_D(ExposureNotification);
    QByteArray dtk;
    QByteArray rpi;
    QByteArray metadata;
    qint8 txPower;

    qDebug() << "CONTRAC: onRpiChanged";
    dtk = d->m_contrac->dtk();
    rpi = d->m_contrac->rpi();
    txPower = Settings::getInstance().txPower();
    d->m_metadata.setDtk(dtk);
    d->m_metadata.setRpi(rpi);
    d->m_metadata.setTxPower(txPower);
    metadata = d->m_metadata.encryptedMetadata();
    if (!d->m_metadata.valid()) {
        qDebug() << "Metadata could not be created";
    }

    d->m_controller->setAdvertData(rpi, metadata);

    emit beaconSent();
}

QDateTime ExposureNotification::lastProcessTime(QString const &token) const
{
    Q_D(const ExposureNotification);

    // Will be an invalid QDateTime if it doesn't exist in the map
    return d->m_lastProcessTime.value(token);
}
