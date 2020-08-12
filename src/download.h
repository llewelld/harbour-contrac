#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QDate>
#include <QMap>

class S3Access;

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

public:
    enum Status
    {
        StatusIdle,
        StatusDownloading,
        StatusError
    };
    enum ErrorType
    {
        ErrorNone,
        ErrorNetwork
    };
    explicit Download(QObject *parent = nullptr);

    Q_INVOKABLE void downloadLatest();

    QDate latest() const;
    bool downloading() const;
    float progress() const;
    Status status() const;
    ErrorType error() const;

signals:
    void latestChanged();
    void downloadingChanged();
    void downloadComplete(QString const &filename);
    void progressChanged();
    void statusChanged();
    void errorChanged();

private slots:
    void setStatus(Status status);

private:
    void addToFileQueue(QDate const &date, QStringList const& download);
    void startNextFileDownload();
    void startNextDateDownload();
    void startDateDownload(QDate const &date);
    QDate nextDownloadDay() const;
    QDate oldestDateInQueue();
    void createDateFolder(QDate const &date) const;
    void finalise();
    void setStatusError(ErrorType error);

private:
    S3Access *m_s3Access;
    QMap<QDate, QStringList> m_fileQueue;
    QDate m_latest;
    bool m_downloading;
    qint64 m_filesReceived;
    qint64 m_filesTotal;
    Status m_status;
    ErrorType m_error;
};

#endif // DOWNLOAD_H
