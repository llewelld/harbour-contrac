#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QStringList>

#include "appsettings.h"
#include "downloadconfig.h"
#include "serveraccess.h"

#include "download.h"

#define DAYS_START_OFFSET (-14)
#define DAYS_TO_DOWNLOAD (13)

namespace {

QDate latestDownloaded()
{
    QDir root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/download/";
    QDate latest;

    root.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    root.setNameFilters(QStringList(QStringLiteral("\?\?\?\?-\?\?-\?\?")));
    root.setSorting(QDir::Name);
    QFileInfoList const &dirs = root.entryInfoList();
    QDate const today = QDate::currentDate();

    QRegExp dateFormat("(\\d{4})-(\\d{2})-(\\d{2})");
    for (QFileInfo const &dir : dirs) {
        if (dateFormat.exactMatch(dir.fileName()) && (dateFormat.captureCount() == 3)) {
            // Valid download folder

            QStringList const elements = dateFormat.capturedTexts();
            int year = elements[1].toInt();
            int month = elements[2].toInt();
            int day = elements[3].toInt();

            if ((year != 0) && (month != 0) && (day != 0)) {
                QDate const date(year, month, day);

                // Don't include dates from today and onwards
                // Since a download on the day may not be complete
                if ((date < today) && (!latest.isValid() || (date > latest))) {
                    latest = date;
                }
            }
            else {
                qDebug() << "Incorrect date";
            }
        }
    }

    qDebug() << "Latest download directory date: " << latest.toString();
    return latest;
}

} // Anonymous namespace

Download::Download(QObject *parent)
    : QObject(parent)
    , m_serverAccess(new ServerAccess(this))
    , m_fileQueue()
    , m_latest()
    , m_downloading(false)
    , m_filesReceived(0)
    , m_filesTotal(0)
    , m_status(StatusIdle)
    , m_downloadConfig(new DownloadConfig(this))
    , m_countryCode()
    , m_downloadedPreviously()
    , m_fileProgress(0.0)
{
    m_serverAccess->setId("accessKey1");
    m_serverAccess->setSecret("verySecretKey1");
    m_serverAccess->setBaseUrl(AppSettings::getInstance().downloadServer());
    m_serverAccess->setBucket("cwa");

    // Read from the filesystem
    m_latest = latestDownloaded();

    connect(m_downloadConfig, &DownloadConfig::configChanged, this, &Download::configChanged);
    connect(m_downloadConfig, &DownloadConfig::downloadComplete, this, &Download::configDownloadComplete);
}

void Download::cleanUpDownloads()
{
    QDir root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/download/";

    root.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    root.setNameFilters(QStringList(QStringLiteral("\?\?\?\?-\?\?-\?\?")));
    root.setSorting(QDir::Name);
    QFileInfoList const &dirs = root.entryInfoList();
    QDate const earliest = QDate::currentDate().addDays(DAYS_START_OFFSET);

    if (earliest.isValid()) {
        for (QFileInfo const &dir : dirs) {
            QDate const date = QDate::fromString(dir.fileName(), "yyyy-MM-dd");
            if (date.isValid()) {
                // Valid download folder
                if (date < earliest) {
                    // Delete the directory
                    QDir directory(dir.absoluteFilePath());
                    qDebug() << "Deleting old directory:" << directory.path();
                    directory.removeRecursively();
                }
                else {
                    qDebug() << "Keys downloaded previously:" << date.toString();
                    m_downloadedPreviously += date;
                }
            }
            else {
                qDebug() << "Incorrect date:" << dir.fileName();
            }
        }
    }
    else {
        qDebug() << "Earliest date could not be determined";
    }
}

void Download::downloadLatest()
{
    if (!m_downloading) {
        // Clean up the old folders
        cleanUpDownloads();

        // Copy the country code to prevent changes mid-download
        m_countryCode = AppSettings::getInstance().countryCode();

        m_filesReceived = 0;
        m_fileProgress = 0.0;
        m_filesTotal = DAYS_TO_DOWNLOAD - m_downloadedPreviously.size();
        m_downloading = true;
        setStatus(StatusDownloadingConfig);
        emit downloadingChanged();
        emit progressChanged();

        m_downloadConfig->downloadLatest();
    }
    else {
        qDebug() << "Download ongoing; skipping download request";
    }
}

void Download::configDownloadComplete(QString const &)
{
    switch (m_downloadConfig->error()) {
    case DownloadConfig::ErrorNone:
        qDebug() << "Requesting keys";
        setStatus(StatusDownloadingKeys);
        m_serverAccess->setBaseUrl(AppSettings::getInstance().downloadServer());
        downloadDateList();
        break;
    default:
        qDebug() << "Network error while downloading configuration: " << m_downloadConfig->error();
        finalise();
        setStatusError(ErrorNetwork);
        break;
    }
}

void Download::downloadDateList()
{
    qDebug() << "Downloading list of available dates";

    QString url = "version/v1/diagnosis-keys/country/" + m_countryCode + "/date";
    ServerListResult *result = m_serverAccess->list(url);
    connect(result, &ServerListResult::finished, this, [this, result]() {
        QStringList dates;

        switch (result->error()) {
        case QNetworkReply::NoError:
            dates = result->keys();
            qDebug() << "Dates available: " << dates.size();
            for (QString date : dates) {
                QDate dateValue = QDate::fromString(date, "yyyy-MM-dd");
                QDate const earliest = QDate::currentDate().addDays(DAYS_START_OFFSET);
                if (m_downloadedPreviously.contains(dateValue)) {
                    qDebug() << "Skipping as already downloaded: " << date;
                }
                else {
                    if (dateValue < earliest) {
                        qDebug() << "Skipping as too old: " << date;
                    }
                    else {
                        createDateFolder(dateValue);
                        qDebug() << "Adding to download queue: " << date;
                        addToFileQueue(dateValue.toString("yyyy-MM-dd"));
                    }
                }
            }
            m_filesTotal = m_fileQueue.size();
            if (m_fileQueue.isEmpty()) {
                finalise();
                setStatus(StatusIdle);
                emit allFilesDownloaded();
            }
            break;
        default:
            qDebug() << "Network error while downloading dates list: " << result->error();
            finalise();
            setStatusError(ErrorNetwork);
            break;
        }

        result->deleteLater();
    });
}

void Download::addToFileQueue(QString const &download)
{
    int size = m_fileQueue.size();
    if (!download.isEmpty()) {
        m_fileQueue += download;
    }
    if (size == 0) {
        startNextFileDownload();
    }
}

void Download::startNextFileDownload()
{
    if (m_fileQueue.isEmpty()) {
        // We've downloaded all the files
        qDebug() << "All files downloaded";
        finalise();
        setStatus(StatusIdle);
        emit allFilesDownloaded();
    }
    else {
        qDebug() << "Starting file download";
        QString name = m_fileQueue.first();
        QString filename = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/download/" + name + "/all.dat";
        qDebug() << "Downloading" << name << "to:" << filename;
        QString url = "version/v1/diagnosis-keys/country/" + m_countryCode + "/date/" + name;
        ServerGetFileResult *result = m_serverAccess->getFile(url, filename);
        connect(result, &ServerResult::progress, this, &Download::fileProgress);
        connect(result, &ServerResult::finished, this, [this, result, name, filename]() {
            qDebug() << "Finished downloading:" << name;

            m_fileQueue.removeFirst();
            ++m_filesReceived;
            m_fileProgress = 0.0;
            QDate date = QDate::fromString(name, "yyyy-MM-dd");
            if (date > m_latest) {
                m_latest = date;
                emit latestChanged();
            }
            emit downloadComplete(filename);
            emit progressChanged();

            startNextFileDownload();
            result->deleteLater();
        });
    }
}

QDate Download::latest() const
{
    return m_latest;
}

void Download::createDateFolder(QDate const &date) const
{
    QDir dir;
    QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/download/" + date.toString("yyyy-MM-dd");
    qDebug() << "Creating directory:" << folder;
    dir.mkpath(folder);
}

bool Download::downloading() const
{
    return m_downloading;
}

float Download::progress() const
{
    float progress = 0.0f;
    if (m_filesTotal > 0) {
        progress = static_cast<float>(m_filesReceived) / static_cast<float>(m_filesTotal);
        progress += m_fileProgress / static_cast<float>(m_filesTotal);
    }

    return progress;
}

void Download::finalise()
{
    if (m_downloading) {
        m_downloading = false;
        m_filesTotal = 0;
        m_filesReceived = 0;

        emit downloadingChanged();
    }
    emit progressChanged();
}

Download::Status Download::status() const
{
    return m_status;
}

void Download::setStatus(Status status)
{
    if (m_status != status) {
        qDebug() << "Setting status: " << status;
        m_status = status;

        if (m_error != ErrorNone) {
            m_error = ErrorNone;
            emit errorChanged();
        }
        emit statusChanged();
    }
}

Download::ErrorType Download::error() const
{
    return m_error;
}

void Download::setStatusError(ErrorType error)
{
    if (m_error != error) {
        qDebug() << "Setting error: " << error;
        m_error = error;

        if (m_status != StatusError) {
            m_status = StatusError;
            emit statusChanged();
        }
        emit errorChanged();
    }
}

ExposureConfiguration *Download::config() const
{
    qDebug() << "Minimum risk score set to: " << m_downloadConfig->config()->minimumRiskScore();
    qDebug() << "Attenuation scores set to: " << m_downloadConfig->config()->attenuationScores();
    qDebug() << "Days Since Last Exposure scores set to: " << m_downloadConfig->config()->daysSinceLastExposureScores();
    qDebug() << "Duration scores set to: " << m_downloadConfig->config()->durationScores();
    qDebug() << "Transmission Risk scores set to: " << m_downloadConfig->config()->transmissionRiskScores();
    qDebug() << "Duration At Attenuation Thresholds set to: " << m_downloadConfig->config()->durationAtAttenuationThresholds();

    return m_downloadConfig->config();
}

QStringList Download::fileList() const
{
    QDir root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/download/";
    QStringList result;

    root.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    root.setNameFilters(QStringList(QStringLiteral("\?\?\?\?-\?\?-\?\?")));
    root.setSorting(QDir::Name);
    QFileInfoList dirs = root.entryInfoList();

    QRegExp dateFormat("\\d{4}-\\d{2}-\\d{2}");
    QRegExp fileFormat("all\\.dat");
    for (QFileInfo dir : dirs) {
        if (dateFormat.exactMatch(dir.fileName())) {
            QDir day(dir.absoluteFilePath());
            day.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
            day.setNameFilters(QStringList(QStringLiteral("all.dat")));
            day.setSorting(QDir::Name);
            QFileInfoList files = day.entryInfoList();
            for (QFileInfo file : files) {
                if (fileFormat.exactMatch(file.fileName())) {
                    result << file.absoluteFilePath();
                }
            }
        }
    }

    return result;
}

void Download::clearError()
{
    if (m_error != ErrorNone) {
        qDebug() << "Clearing download error status";
        m_error = ErrorNone;

        if (m_status == StatusError) {
            m_status = StatusIdle;
            emit statusChanged();
        }
        emit errorChanged();
    }
}

void Download::fileProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    float previousProgress = progress();
    if (bytesTotal > 0) {
        m_fileProgress = (double) bytesReceived / (double) bytesTotal;
        if (progress() > previousProgress) {
            emit progressChanged();
        }
    }
}
