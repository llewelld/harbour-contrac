#include <iostream>
#include <fstream>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qdebug.h>

#include "proto/submissionpayload.pb.h"
#include <openssl/evp.h>
#include "upload.h"

#define UPLOAD_SERVER_ADDRESS "127.0.0.1:8000"
#define VERIFICATION_SERVER_ADDRESS "127.0.0.1:8004"

Upload::Upload(QObject *parent) : QObject(parent)
  , m_manager(new QNetworkAccessManager(this))
  , m_reply(nullptr)
  , m_latest()
  , m_bytesSent(0)
  , m_bytesTotal(0)
  , m_status(StatusIdle)
{
}

void Upload::upload(QString const &teleTAN)
{
    if (((m_status == StatusIdle) || (m_status == StatusError)) && (m_reply == nullptr)) {
        bool valid = validateTeleTAN(teleTAN);
        if (valid) {
            setStatus(StatusSubmitTeleTAN);
            submitTeleTAN(teleTAN);
        }
        else {
            qDebug() << "Invalid teleTAN";
            setStatus(StatusError);
        }
    }
    else {
        // Do nothing until the existing upload completes
        qDebug() << "Upload already in progress";
    }
}

void Upload::submitTeleTAN(QString const &teleTAN)
{
    if ((m_status == StatusSubmitTeleTAN) && (m_reply == nullptr)) {
        QNetworkRequest request;

        request.setUrl(QUrl("http://" VERIFICATION_SERVER_ADDRESS "/version/v1/registrationToken"));
        request.setRawHeader("accept", "*/*");
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("cwa-fake", "0");

        QJsonObject body;
        body.insert("keyType", "TELETAN");
        body.insert("key", teleTAN);
        QJsonDocument doc(body);
        QByteArray data = doc.toJson();
        m_reply = m_manager->post(request, data);

        connect(m_reply, &QNetworkReply::finished, this, &Upload::onTeleTANFinished);
    }
    else {
        qDebug() << "Incorrect state attempting to submit teleTAN";
        setStatus(StatusError);
    }
}

void Upload::onTeleTANFinished()
{
    QNetworkReply *reply = m_reply;
    m_reply = nullptr;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        if (doc.isNull()) {
            qDebug() << "JSON error: " << error.errorString();
            setStatus(StatusError);
        }
        else {
            QString regToken = doc.object().value("registrationToken").toString();
            if (regToken.isEmpty()) {
                qDebug() << "Invalid registration token";
                setStatus(StatusError);
            }
            else {
                setStatus(StatusSubmitRegToken);
                submitRegToken(regToken);
            }
        }
    }
    else {
        qDebug() << "Network error while submitting teleTAN: " << reply->errorString();
        setStatus(StatusError);
    }
    reply->deleteLater();
}

void Upload::submitRegToken(QString const &regToken)
{
    if ((m_status == StatusSubmitRegToken) && (m_reply == nullptr)) {
        QNetworkRequest request;

        request.setUrl(QUrl("http://" VERIFICATION_SERVER_ADDRESS "/version/v1/tan"));
        request.setRawHeader("accept", "*/*");
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("cwa-fake", "0");

        QJsonObject body;
        body.insert("registrationToken", regToken);
        QJsonDocument doc(body);
        QByteArray data = doc.toJson();
        m_reply = m_manager->post(request, data);

        connect(m_reply, &QNetworkReply::finished, this, &Upload::onRegTokenFinished);
    }
    else {
        qDebug() << "Incorrect state attempting to submit registration token";
        setStatus(StatusError);
    }
}

void Upload::onRegTokenFinished()
{
    QNetworkReply *reply = m_reply;
    m_reply = nullptr;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        if (doc.isNull()) {
            qDebug() << "JSON error: " << error.errorString();
            setStatus(StatusError);
        }
        else {
            QString tan = doc.object().value("tan").toString();
            if (tan.isEmpty()) {
                qDebug() << "Invalid TAN";
                setStatus(StatusError);
            }
            else {
                setStatus(SubmitDiagnosisKeys);
                submitDiagnosisKeys(tan);
            }
        }
    }
    else {
        qDebug() << "Network error while submitting registration token: " << reply->errorString();
        setStatus(StatusError);
    }
    reply->deleteLater();
}

void Upload::submitDiagnosisKeys(QString const &tan)
{
    if ((m_status == SubmitDiagnosisKeys) && (m_reply == nullptr)) {
        diagnosis::SubmissionPayload payload;
        QNetworkRequest request;
        DBusProxy proxy(this);

        // Get the latest keys for upload
        QList<TemporaryExposureKey> exposureKeys = proxy.getTemporaryExposureKeyHistory();

        qDebug() << "Generating submission payload";
        qDebug() << "Keys to output: " << exposureKeys.length();

        // Convert the keys into protobuffer format
        for (TemporaryExposureKey const &key : exposureKeys) {
            diagnosis::TemporaryExposureKey *keys= payload.add_keys();
            keys->set_key_data(key.keyData().data(), static_cast<size_t>(key.keyData().length()));
            keys->set_rolling_start_interval_number(static_cast<qint32>(key.rollingStartNumber()));
            keys->set_rolling_period(static_cast<qint32>(key.rollingPeriod()));
            keys->set_transmission_risk_level(key.transmissionRiskLevel());

            QDateTime time;
            time.setMSecsSinceEpoch(0);
            QDate date = time.addSecs(key.rollingStartNumber() * 60 * 10).date();
            if (date > m_latest) {
                m_latest = date;
                emit latestChanged();
            }
        }
        qDebug() << "Keys included: " << payload.keys_size();

        QByteArray data = QByteArray::fromStdString(payload.SerializeAsString());
        qDebug() << "Upload size: " << data.length();

        request.setUrl(QUrl("http://" UPLOAD_SERVER_ADDRESS "/version/v1/diagnosis-keys"));
        request.setRawHeader("accept", "*/*");
        request.setRawHeader("Content-Type", "application/x-protobuf");
        request.setRawHeader("cwa-authorization", tan.toLatin1());
        request.setRawHeader("cwa-fake", "0");
        m_reply = m_manager->post(request, data);
        qDebug() << "Uploading";

        m_bytesSent = 0;
        m_bytesTotal = data.size();
        connect(m_reply, &QNetworkReply::finished, this, &Upload::onDiagnosisKeysFinished);
        connect(m_reply, &QNetworkReply::uploadProgress, this, &Upload::onProgress);
        emit uploadingChanged();
    }
    else {
        qDebug() << "Incorrect state attempting to submit diagnosis keys";
        setStatus(StatusError);
    }
}

void Upload::onDiagnosisKeysFinished()
{
    QNetworkReply *reply = m_reply;
    m_reply = nullptr;

    qDebug() << "Upload finished";

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Upload successful";
        setStatus(StatusIdle);
    }
    else {
        qDebug() << "Network error while submitting diagnosis keys: " << reply->errorString();
        setStatus(StatusError);
    }

    reply->deleteLater();
    emit uploadingChanged();
}

bool Upload::uploading() const
{
    return (m_reply != nullptr);
}

float Upload::progress() const
{
    float progress = 0.0f;

    if (m_bytesTotal > 0.0f) {
        progress = static_cast<float>(m_bytesSent) / static_cast<float>(m_bytesTotal);
    }

    return progress;
}

Upload::Status Upload::status() const
{
    return m_status;
}

void Upload::setStatus(Status status)
{
    if (m_status != status) {
        qDebug() << "Setting status: " << status;
        m_status = status;
        emit statusChanged();
    }
}

void Upload::onProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << "CONTRAC: bytes sent: " << bytesSent;
    qDebug() << "CONTRAC: bytes total: " << bytesTotal;

    m_bytesSent = bytesSent;
    m_bytesTotal = bytesTotal;

    emit progressChanged();
}

QDate Upload::latest() const
{
    return m_latest;
}

bool Upload::validateTeleTAN(QString const &teleTAN)
{
    static const uint16_t length = 10;
    bool result;
    QByteArray data;

    qDebug() << "Validating teleTAN: " << teleTAN;
    result = teleTAN.length() == length;

    qDebug() << "Valid Length: " << result;

    for (uint16_t pos = 0; pos < length && result; ++pos) {
        result &= validateTeleTANCharacter(teleTAN.at(pos));
    }

    qDebug() << "Valid characters: " << result;

    if (result) {
        EVP_MD_CTX *context;
        char hash[EVP_MAX_MD_SIZE];
        unsigned int hashLength;

        context = EVP_MD_CTX_create();
        EVP_DigestInit_ex(context, EVP_sha256(), nullptr);
        data = teleTAN.left(length - 1).toLatin1();
        EVP_DigestUpdate(context, data.data(), static_cast<unsigned int>(data.length()));

        EVP_DigestFinal_ex(context, reinterpret_cast<unsigned char *>(hash), &hashLength);
        EVP_MD_CTX_destroy(context);

        result = hashLength > 0;

        if (result) {
            QChar hashCheck = QByteArray(hash, static_cast<int>(hashLength)).toHex().at(0);
            if (hashCheck == '0') {
                hashCheck = 'g';
            }
            if (hashCheck == '1') {
                hashCheck = 'h';
            }
            result = (teleTAN.at(length - 1).toLower() == hashCheck);

            qDebug() << "Valid checksum: " << result;
        }
    }

    return result;
}

bool Upload::validateTeleTANCharacter(QChar const &character)
{
    static const QString validChars = "23456789ABCDEFGHJKMNPQRSTUVWXYZ";

    return validChars.contains(character, Qt::CaseInsensitive);
}
