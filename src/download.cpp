#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "appsettings.h"
#include "serveraccess.h"
#include "downloadconfig.h"

#include "download.h"

#define DAYS_START_OFFSET (-15)
#define DAYS_TO_DOWNLOAD (14)

namespace {

void cleanUpDownloads()
{
    QDir root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/download/";

    root.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    root.setNameFilters(QStringList(QStringLiteral("\?\?\?\?-\?\?-\?\?")));
    root.setSorting(QDir::Name);
    QFileInfoList const &dirs = root.entryInfoList();
    QDate const earliest = QDate::currentDate().addDays(DAYS_START_OFFSET);

    if (earliest.isValid()) {
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

                    if (date < earliest) {
                        // Delete the directory
                        QDir directory(dir.absoluteFilePath());
                        qDebug() << "Deleting old directory: " << directory.path();
                        directory.removeRecursively();
                    }
                }
                else {
                    qDebug() << "Incorrect date";
                }
            }
        }
    }
    else {
        qDebug() << "Earliest date could not be determined";
    }
}

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

    qDebug() << "Latest download directory date: " << latest;
    return latest;
}

} // Anonymous namespace

Download::Download(QObject *parent) : QObject(parent)
  , m_serverAccess(new ServerAccess(this))
  , m_fileQueue()
  , m_latest()
  , m_downloading(false)
  , m_filesReceived(0)
  , m_filesTotal(0)
  , m_status(StatusIdle)
  , m_downloadConfig(new DownloadConfig(this))
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

Q_INVOKABLE void Download::downloadLatest()
{
    qint64 daysTotal;

    if (!m_downloading) {
        // Clean up the old folders
        cleanUpDownloads();

        if (m_latest.isValid()) {
            daysTotal = m_latest.daysTo(QDate::currentDate());
        }
        else{
            daysTotal = DAYS_TO_DOWNLOAD;
        }
        if (daysTotal > DAYS_TO_DOWNLOAD) {
            daysTotal = DAYS_TO_DOWNLOAD;
        }
        m_filesReceived = 0;
        m_filesTotal = daysTotal * 24;
        m_downloading = true;
        setStatus(StatusDownloadingConfig);
        emit downloadingChanged();
        emit progressChanged();

        m_downloadConfig->downloadLatest();
    }
}

void Download::configDownloadComplete(QString const &)
{
    switch (m_downloadConfig->error()) {
    case DownloadConfig::ErrorNone:
        qDebug() << "Requesting keys";
        setStatus(StatusDownloadingKeys);
        m_serverAccess->setBaseUrl(AppSettings::getInstance().downloadServer());
        startNextDateDownload();
        break;
    default:
        qDebug() << "Network error while downloading configuration: " << m_downloadConfig->error();
        finalise();
        setStatusError(ErrorNetwork);
        break;
    }
}

QDate Download::nextDownloadDay() const
{
    QDate today = QDate::currentDate();
    QDate date = today.addDays(DAYS_START_OFFSET);
    QDate next = m_latest.addDays(1);
    if (next > date) {
        date = next;
    }
    if (date >= today) {
        date = QDate();
    }

    return date;
}

void Download::addToFileQueue(QDate const &date, QStringList const &download)
{
    int size = m_fileQueue.size();
    if (download.size() > 0) {
        m_fileQueue[date] += download;
    }
    if (size == 0) {
        startNextFileDownload();
    }
}

QDate Download::oldestDateInQueue()
{
    qDebug() << "Queue size before: " << m_fileQueue.size();
    QDate oldest;
    for (QDate const &key : m_fileQueue.keys()) {
        if (m_fileQueue[key].size() == 0) {
            m_fileQueue.remove(key);
        }
        else {
            if (!oldest.isValid() || key < oldest) {
                oldest = key;
            }
        }
    }

    qDebug() << "Queue size after: " << m_fileQueue.size();
    qDebug() << "Oldest date: " << oldest;

    return oldest;
}

void Download::startNextFileDownload() {
    qDebug() << "Starting file download";
    QDate date = oldestDateInQueue();
    if (date.isValid()) {
        QString key = m_fileQueue[date].first();

        // FIXME: Use the actual date rather than the URL to figure out the filename
        int dateEnd = key.lastIndexOf("/", key.lastIndexOf("/") - 1) - 1;
        int dateStart = key.lastIndexOf("/", dateEnd) + 1;
        QString file = key.mid(dateStart, dateEnd - dateStart + 1) + "/" + key.mid(key.lastIndexOf("/") + 1);

        QString filename = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/download/" + file;
        qDebug() << "Saving to:" << filename;
        ServerGetFileResult *result = m_serverAccess->getFile(key, filename);
        connect(result, &ServerResult::finished, this, [this, result, date, filename]() {
            qDebug() << "Finished downloading:" << m_fileQueue[date].first();

            ++m_filesReceived;
            emit downloadComplete(filename);
            emit progressChanged();

            m_fileQueue[date].removeFirst();
            if (m_fileQueue[date].size() == 0) {
                // We've downloaded all the files for this day
                qDebug() << "All files downloaded for:" << date;
                m_fileQueue.remove(date);
                m_latest = date;
                emit latestChanged();
            }
            startNextFileDownload();
            result->deleteLater();
        });
    }
    else {
        qDebug() << "File queue empty";
        if (m_downloading) {
            startNextDateDownload();
        }
    }
}

void Download::startNextDateDownload()
{
    QDate date = nextDownloadDay();
    bool downloading = m_downloading;

    if (date.isValid()) {
        m_downloading = true;
        startDateDownload(date);
    }
    else {
        qDebug() << "All dates downloaded";
        finalise();
        downloading = m_downloading;
        setStatus(StatusIdle);
        emit allFilesDownloaded();
    }

    if (m_downloading != downloading) {
        emit downloadingChanged();
    }
}

void Download::startDateDownload(QDate const &date)
{
    qDebug() << "Starting date download:" << date;

    QString url = "version/v1/diagnosis-keys/country/DE/date/" + date.toString("yyyy-MM-dd") + "/hour";
    ServerListResult *result = m_serverAccess->list(url);
    connect(result, &ServerListResult::finished, this, [this, result, date]() {
        QStringList keys;

        switch (result->error()) {
        case QNetworkReply::NoError:
            keys = result->keys();
            if (keys.size() > 0) {
                qDebug() << "Key list completed: " << keys.size();
                createDateFolder(date);
                for (QString key : keys) {
                    qDebug() << "Key: " << key;
                }
                addToFileQueue(date, keys);
            }
            else {
                m_latest = date;
                emit latestChanged();
                startNextDateDownload();
            }
            break;
        default:
            qDebug() << "Network error while downloading keys: " << result->error();
            finalise();
            setStatusError(ErrorNetwork);
            break;
        }

        result->deleteLater();
    });
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

ExposureConfiguration const *Download::config() const
{
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
    QRegExp hourFormat("\\d{1,2}");
    for (QFileInfo dir : dirs) {
        if (dateFormat.exactMatch(dir.fileName())) {
            QDir day(dir.absoluteFilePath());
            day.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
            day.setNameFilters(QStringList(QStringLiteral("\?\?")) << QStringLiteral("\?"));
            day.setSorting(QDir::Name);
            QFileInfoList hours = day.entryInfoList();
            for (QFileInfo hour : hours) {
                if (hourFormat.exactMatch(hour.fileName())) {
                    result << hour.absoluteFilePath();
                }
            }
        }
    }

    return result;
}

Q_INVOKABLE void Download::clearError()
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
