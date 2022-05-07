#ifndef UPLOAD_H
#define UPLOAD_H

#include <QNetworkReply>
#include <QObject>

#include "contracd/src/temporaryexposurekey.h"
#include "dbusproxy.h"

class Upload : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(ErrorType)
    Q_PROPERTY(QDate latest READ latest NOTIFY latestChanged)
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(ErrorType error READ error NOTIFY errorChanged)

public:
    enum Status
    {
        StatusIdle,
        StatusSubmitTeleTAN,
        StatusSubmitGUID,
        CheckForTestResult,
        StatusSubmitRegToken,
        SubmitDiagnosisKeys,
        StatusError
    };
    enum ErrorType
    {
        ErrorNone,
        ErrorInvalidteleTAN,
        ErrorInvalidGUID,
        ErrorInvalidRegToken,
        ErrorInvalidTestResult,
        ErrorInvalidDiagnosisKeys,
        ErrorNoDiagnosisKeys,
        ErrorNetwork,
        ErrorParsing,
        ErrorInternal
    };
    explicit Upload(QObject *parent = nullptr);

    Q_INVOKABLE void upload(QString const &teleTAN);
    Q_INVOKABLE bool validateTeleTAN(QString const &teleTAN) const;
    Q_INVOKABLE bool validateTeleTANCharacter(QChar const &character) const;
    Q_INVOKABLE bool validateTeleTANCharacters(QString const &teleTAN) const;
    Q_INVOKABLE void uploadGUID(QString guid);
    Q_INVOKABLE bool validateGUID(QString guid) const;
    Q_INVOKABLE void checkForTestResult(QString const &regToken);
    Q_INVOKABLE void submitKeysAfterPositiveResult();
    float progress() const;
    bool uploading() const;
    QDate latest() const;
    Status status() const;
    ErrorType error() const;
    Q_INVOKABLE void clearError();

signals:
    void uploadComplete();
    void progressChanged();
    void uploadingChanged();
    void latestChanged();
    void statusChanged();
    void errorChanged();
    void regTokenStored(const QDate &date);
    void testResultRetrieved(int result);
    void diagnosisKeysSubmittedSuccessfully();

private slots:
    void setStatus(Status status);
    void onTeleTANFinished();
    void onGUIDFinished();
    void onCheckForTestResultFinished();
    void onRegTokenFinished();
    void onDiagnosisKeysFinished();
    void onProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    void sanitizeQrCodeURL(QString &guid) const;
    void submitTeleTAN(QString const &teleTAN);
    void submitGUID(QString guid);
    void submitRegToken(QString const &regToken);
    void submitDiagnosisKeys(QString const &tan);
    void setStatusError(ErrorType error);

private:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QDate m_latest;
    qint64 m_bytesSent;
    qint64 m_bytesTotal;
    Status m_status;
    ErrorType m_error;
};

#endif // UPLOAD_H
