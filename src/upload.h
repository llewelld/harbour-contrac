#ifndef UPLOAD_H
#define UPLOAD_H

#include <QObject>
#include <QNetworkReply>

#include "dbusproxy.h"
#include "contracd/src/temporaryexposurekey.h"

class Upload : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(QDate latest READ latest NOTIFY latestChanged)
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    enum Status
    {
        StatusIdle,
        StatusSubmitTeleTAN,
        StatusSubmitRegToken,
        SubmitDiagnosisKeys,
        StatusError
    };
    explicit Upload(QObject *parent = nullptr);

    Q_INVOKABLE void upload(QString const &teleTAN);
    Q_INVOKABLE bool validateTeleTAN(QString const &teleTAN);
    Q_INVOKABLE bool validateTeleTANCharacter(QChar const &character);
    float progress() const;
    bool uploading() const;
    QDate latest() const;
    Status status() const;

signals:
    void uploadComplete();
    void progressChanged();
    void uploadingChanged();
    void latestChanged();
    void statusChanged();

private slots:
    void setStatus(Status status);
    void onTeleTANFinished();
    void onRegTokenFinished();
    void onDiagnosisKeysFinished();
    void onProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    void submitTeleTAN(QString const &teleTAN);
    void submitRegToken(QString const &regToken);
    void submitDiagnosisKeys(QString const &tan);

private:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QDate m_latest;
    qint64 m_bytesSent;
    qint64 m_bytesTotal;
    Status m_status;
};

#endif // UPLOAD_H
