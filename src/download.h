#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QDate>
#include <QMap>
#include <QSet>

#include "../contracd/src/exposureconfiguration.h"

class ServerAccess;
class DownloadConfig;

class Download : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(ErrorType)
    Q_PROPERTY(QDate latest READ latest NOTIFY latestChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(ErrorType error READ error NOTIFY errorChanged)
    Q_PROPERTY(ExposureConfiguration *config READ config NOTIFY configChanged)

public:
    enum Status
    {
        StatusIdle,
        StatusDownloadingConfig,
        StatusDownloadingKeys,
        StatusError
    };
    enum ErrorType
    {
        ErrorNone,
        ErrorNetwork
    };
    explicit Download(QObject *parent = nullptr);

    Q_INVOKABLE void downloadLatest();
    Q_INVOKABLE QStringList fileList() const;

    QDate latest() const;
    bool downloading() const;
    float progress() const;
    Status status() const;
    ErrorType error() const;
    Q_INVOKABLE void clearError();
    ExposureConfiguration *config() const;

signals:
    void latestChanged();
    void downloadingChanged();
    void downloadComplete(QString const &filename);
    void progressChanged();
    void statusChanged();
    void errorChanged();
    void configChanged();
    void allFilesDownloaded();

private slots:
    void setStatus(Status status);
    void configDownloadComplete(QString const &filename);
    void fileProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    void downloadDateList();
    void addToFileQueue(QString const& download);
    void startNextFileDownload();
    void createDateFolder(QDate const &date) const;
    void finalise();
    void setStatusError(ErrorType error);
    void cleanUpDownloads();

private:
    ServerAccess *m_serverAccess;
    QStringList m_fileQueue;
    QDate m_latest;
    bool m_downloading;
    qint64 m_filesReceived;
    qint64 m_filesTotal;
    Status m_status;
    ErrorType m_error;
    DownloadConfig * m_downloadConfig;
    QString m_countryCode;
    QSet<QDate> m_downloadedPreviously;
    float m_fileProgress;
};

#endif // DOWNLOAD_H
