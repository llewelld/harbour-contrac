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
    Q_PROPERTY(QDate latest READ latest NOTIFY latestChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    enum Status
    {
        StatusIdle,
        StatusDownloading,
        StatusError
    };

    explicit Download(QObject *parent = nullptr);

    Q_INVOKABLE void downloadLatest();

    QDate latest() const;
    bool downloading() const;
    float progress() const;
    Status status() const;

signals:
    void latestChanged();
    void downloadingChanged();
    void downloadComplete(QString const &filename);
    void progressChanged();
    void statusChanged();

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

private:
    S3Access *m_s3Access;
    QMap<QDate, QStringList> m_fileQueue;
    QDate m_latest;
    bool m_downloading;
    qint64 m_filesReceived;
    qint64 m_filesTotal;
    Status m_status;
};

#endif // DOWNLOAD_H
