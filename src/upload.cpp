#include <fstream>
#include <iostream>
#include <qcryptographichash.h>
#include <qdebug.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

#include <openssl/evp.h>

#include "appsettings.h"
#include "sfsecrethelper.h"
#include "submissionpayload.pb.h"
#include "testresult.h"
#include "upload.h"

static constexpr uint16_t GUID_LENGTH = 38;

Upload::Upload(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_reply(nullptr)
    , m_latest()
    , m_bytesSent(0)
    , m_bytesTotal(0)
    , m_status(StatusIdle)
    , m_error(ErrorNone)
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
            setStatusError(ErrorInvalidteleTAN);
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

        request.setUrl(QUrl(AppSettings::getInstance().verificationServer() + "/version/v1/registrationToken"));
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
        setStatusError(ErrorInternal);
    }
}

void Upload::onTeleTANFinished()
{
    QNetworkReply *reply;
    QByteArray data;
    QJsonParseError error;
    QJsonDocument doc;

    reply = m_reply;
    m_reply = nullptr;

    switch (reply->error()) {
    case QNetworkReply::NoError:
        data = reply->readAll();

        doc = QJsonDocument::fromJson(data, &error);
        if (doc.isNull()) {
            qDebug() << "JSON error: " << error.errorString();
            setStatusError(ErrorParsing);
        }
        else {
            QString regToken = doc.object().value("registrationToken").toString();
            if (regToken.isEmpty()) {
                qDebug() << "Invalid registration token";
                setStatusError(ErrorInvalidRegToken);
            }
            else {
                setStatus(StatusSubmitRegToken);
                submitRegToken(regToken);
            }
        }
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        qDebug() << "Server reply: invalid teleTAN";
        setStatusError(ErrorInvalidteleTAN);
        break;
    default:
        qDebug() << "Network error while submitting teleTAN: " << reply->error();
        setStatusError(ErrorNetwork);
        break;
    }
    reply->deleteLater();
}

void Upload::uploadGUID(QString const &guid)
{
    if (((m_status == StatusIdle) || (m_status == StatusError)) && (m_reply == nullptr)) {
        bool valid = validateGUID(guid);
        if (valid) {
            setStatus(StatusSubmitGUID);
            submitGUID(guid);
        }
        else {
            qDebug() << "Invalid GUID";
            setStatusError(ErrorInvalidGUID);
        }
    }
    else {
        // Do nothing until the existing upload completes
        qDebug() << "Upload already in progress";
    }
}

void Upload::submitGUID(QString guid)
{
    guid.insert(6, "-");
    guid.insert(15, "-");
    guid.insert(20, "-");
    guid.insert(25, "-");
    guid.insert(30, "-");
    qDebug() << "GUID with dashes: " << guid;
    QCryptographicHash hasher(QCryptographicHash::Sha256);
    hasher.addData(guid.toLatin1());
    QString hashedGuid = static_cast<QString>(hasher.result().toHex());

    if ((m_status == StatusSubmitGUID) && (m_reply == nullptr) && (hashedGuid.length() > 0)) {
        QNetworkRequest request;

        request.setUrl(QUrl(AppSettings::getInstance().verificationServer() + "/version/v1/registrationToken"));
        request.setRawHeader("accept", "*/*");
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("cwa-fake", "0");

        QJsonObject body;
        body.insert("keyType", "GUID");
        body.insert("key", hashedGuid);
        QJsonDocument doc(body);
        QByteArray data = doc.toJson();
        m_reply = m_manager->post(request, data);

        connect(m_reply, &QNetworkReply::finished, this, &Upload::onGUIDFinished);
    }
    else {
        qDebug() << "Incorrect state attempting to submit GUID";
        setStatusError(ErrorInternal);
    }
}

void Upload::onGUIDFinished()
{
    QNetworkReply *reply;
    QByteArray data;
    QJsonParseError error;
    QJsonDocument doc;

    reply = m_reply;
    m_reply = nullptr;

    switch (reply->error()) {
    case QNetworkReply::NoError:
        data = reply->readAll();

        doc = QJsonDocument::fromJson(data, &error);
        if (doc.isNull()) {
            qDebug() << "JSON error: " << error.errorString();
            setStatusError(ErrorParsing);
        }
        else {
            QString regToken = doc.object().value("registrationToken").toString();
            if (regToken.isEmpty()) {
                qDebug() << "Invalid registration token";
                setStatusError(ErrorInvalidRegToken);
            }
            else {
                if (SFSecretHelper::getInstance().store(REGTOKEN_KEY, regToken.toLatin1()).returncode == 0) {
                    AppSettings::getInstance().setRegTokenReceived(QDate::currentDate());
                    emit regTokenStored(QDate::currentDate());
                    setStatus(StatusIdle);
                }
                else {
                    qDebug() << "SFSecret couldn't store regtoken";
                    setStatusError(ErrorInternal);
                }
            }
        }
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        qDebug() << "Server reply: invalid GUID";
        setStatusError(ErrorInvalidGUID);
        break;
    default:
        qDebug() << "Network error while submitting GUID: " << reply->error();
        setStatusError(ErrorNetwork);
        break;
    }
    reply->deleteLater();
}

void Upload::checkForTestResult(QString const &regToken)
{
    if ((m_status == StatusIdle) && (m_reply == nullptr)) {
        m_status = CheckForTestResult;

        qDebug() << "Checking for testresult with regtoken: " << regToken;
        QNetworkRequest request;

        request.setUrl(QUrl(AppSettings::getInstance().verificationServer() + "/version/v1/testresult"));
        request.setRawHeader("accept", "*/*");
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("cwa-fake", "0");

        QJsonObject body;
        body.insert("registrationToken", regToken);
        QJsonDocument doc(body);
        QByteArray data = doc.toJson();
        m_reply = m_manager->post(request, data);

        connect(m_reply, &QNetworkReply::finished, this, &Upload::onCheckForTestResultFinished);
    }
    else {
        qDebug() << "Incorrect state attempting to check for test result";
        setStatusError(ErrorInternal);
    }
}

void Upload::onCheckForTestResultFinished()
{
    QNetworkReply *reply;
    QByteArray data;
    QJsonParseError error;
    QJsonDocument doc;
    int result = -1;

    reply = m_reply;
    m_reply = nullptr;

    switch (reply->error()) {
    case QNetworkReply::NoError:
        data = reply->readAll();

        doc = QJsonDocument::fromJson(data, &error);
        qDebug() << "Testresult JSON response: " << doc;
        if (doc.isNull()) {
            qDebug() << "JSON error: " << error.errorString();
            setStatusError(ErrorParsing);
        }
        else {
            result = doc.object().value("testResult").toInt();
            if (result == -1) {
                qDebug() << "Result response is empty: -> Invalid testresult";
                setStatus(StatusError);
                setStatusError(ErrorInvalidTestResult);
            }
            else {
                qDebug() << "Retrieved testresult: " << result;
                emit testResultRetrieved(result);
                setStatus(StatusIdle);
            }
        }
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        qDebug() << "Server reply: invalid registration token";
        setStatusError(ErrorInvalidRegToken);
        break;
    default:
        qDebug() << "Network error while submitting registration token: " << reply->error();
        setStatusError(ErrorNetwork);
        break;
    }
    reply->deleteLater();
}

void Upload::submitKeysAfterPositiveResult()
{
    SFSecretHelper::Response res = SFSecretHelper::getInstance().get(REGTOKEN_KEY);
    if (res.returncode == 0) {
        setStatus(StatusSubmitRegToken);
        this->submitRegToken(QString::fromLatin1(res.data));
    }
    else {
        setStatus(StatusError);
        setStatusError(ErrorInternal);
        qWarning() << "Can't submit keys: Couldn't retrieve registry token from Sailfish Secrets.";
    }
}

void Upload::submitRegToken(QString const &regToken)
{
    if ((m_status == StatusSubmitRegToken) && (m_reply == nullptr)) {
        QNetworkRequest request;

        request.setUrl(QUrl(AppSettings::getInstance().verificationServer() + "/version/v1/tan"));
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
        setStatusError(ErrorInternal);
    }
}

void Upload::onRegTokenFinished()
{
    QNetworkReply *reply;
    QByteArray data;
    QJsonParseError error;
    QJsonDocument doc;

    reply = m_reply;
    m_reply = nullptr;

    switch (reply->error()) {
    case QNetworkReply::NoError:
        data = reply->readAll();

        doc = QJsonDocument::fromJson(data, &error);
        if (doc.isNull()) {
            qDebug() << "JSON error: " << error.errorString();
            setStatusError(ErrorParsing);
        }
        else {
            QString tan = doc.object().value("tan").toString();
            if (tan.isEmpty()) {
                qDebug() << "Invalid TAN";
                setStatus(StatusError);
                setStatusError(ErrorInvalidteleTAN);
            }
            else {
                setStatus(SubmitDiagnosisKeys);
                submitDiagnosisKeys(tan);
            }
        }
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        qDebug() << "Server reply: invalid registration token";
        setStatusError(ErrorInvalidRegToken);
        break;
    default:
        qDebug() << "Network error while submitting registration token: " << reply->error();
        setStatusError(ErrorNetwork);
        break;
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

        if (exposureKeys.length() > 0) {
            // Convert the keys into protobuffer format
            for (TemporaryExposureKey const &key : exposureKeys) {
                diagnosis::TemporaryExposureKey *keys = payload.add_keys();
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

            request.setUrl(QUrl(AppSettings::getInstance().uploadServer() + "/version/v1/diagnosis-keys"));
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
            qDebug() << "No diagnosis keys to upload";
            setStatusError(ErrorNoDiagnosisKeys);
        }
    }
    else {
        qDebug() << "Incorrect state attempting to submit diagnosis keys";
        setStatusError(ErrorInternal);
    }
}

void Upload::onDiagnosisKeysFinished()
{
    QNetworkReply *reply = m_reply;
    m_reply = nullptr;

    qDebug() << "Upload finished";

    switch (reply->error()) {
    case QNetworkReply::NoError:
        qDebug() << "Upload successful";
        // Same as TestResult::possiblyAvailable()
        if (AppSettings::getInstance().regTokenReceived().isValid()) {
            AppSettings::getInstance().setRegTokenReceived(QDate(2000, 1, 1));
            emit diagnosisKeysSubmittedSuccessfully();
        }
        setStatus(StatusIdle);
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        qDebug() << "Server reply: invalid diagnosis keys";
        setStatusError(ErrorInvalidDiagnosisKeys);
        break;
    default:
        qDebug() << "Network error while submitting diagnosis keys: " << reply->error();
        setStatusError(ErrorNetwork);
        break;
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

        if (m_error != ErrorNone) {
            m_error = ErrorNone;
            emit errorChanged();
        }
        emit statusChanged();
    }
}

Upload::ErrorType Upload::error() const
{
    return m_error;
}

void Upload::setStatusError(ErrorType error)
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

bool Upload::validateTeleTAN(QString const &teleTAN) const
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

#ifdef OPENSSL_GE_1_1_1
        context = EVP_MD_CTX_new();
#else  // OPENSSL_GE_1_1_1
        context = EVP_MD_CTX_create();
#endif // OPENSSL_GE_1_1_1
        EVP_DigestInit_ex(context, EVP_sha256(), nullptr);
        data = teleTAN.left(length - 1).toLatin1();
        EVP_DigestUpdate(context, data.data(), static_cast<unsigned int>(data.length()));

        EVP_DigestFinal_ex(context, reinterpret_cast<unsigned char *>(hash), &hashLength);
#ifdef OPENSSL_GE_1_1_1
        EVP_MD_CTX_free(context);
#else  // OPENSSL_GE_1_1_1
        EVP_MD_CTX_destroy(context);
#endif // OPENSSL_GE_1_1_1

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

bool Upload::validateTeleTANCharacter(QChar const &character) const
{
    static const QString validChars = "23456789ABCDEFGHJKMNPQRSTUVWXYZ";

    return validChars.contains(character, Qt::CaseInsensitive);
}

bool Upload::validateTeleTANCharacters(QString const &teleTAN) const
{
    int pos;
    bool result;

    result = true;
    pos = 0;
    while (pos < 10 && pos < teleTAN.length() && result) {
        result &= validateTeleTANCharacter(teleTAN.at(pos));
        ++pos;
    }

    return result;
}

bool Upload::validateGUID(QString const &guid) const
{
    bool result;

    qDebug() << "Validating GUID: " << guid;
    result = guid.length() == GUID_LENGTH;

    qDebug() << "Valid Length: " << result;

    for (uint16_t pos = 0; pos < GUID_LENGTH && result; ++pos) {
        result &= validateGUIDCharacter(guid.at(pos));
        qDebug() << "Validating GUID Character: " << guid.at(pos) << " valid: " << result;
    }
    return result;
}

bool Upload::validateGUIDCharacters(QString const &guid) const
{
    int pos;
    bool result;

    result = true;
    pos = 0;
    while (pos < GUID_LENGTH && pos < guid.length() && result) {
        result &= validateGUIDCharacter(guid.at(pos));
        ++pos;
    }

    return result;
}

bool Upload::validateGUIDCharacter(QChar const &character) const
{
    static const QString validChars = "0123456789ABCDEF";
    return validChars.contains(character, Qt::CaseSensitive);
}

Q_INVOKABLE void Upload::clearError()
{
    if (m_error != ErrorNone) {
        qDebug() << "Clearing upload error status";
        m_error = ErrorNone;

        if (m_status == StatusError) {
            m_status = StatusIdle;
            emit statusChanged();
        }
        emit errorChanged();
    }
}
