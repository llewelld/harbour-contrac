#include <QFile>

#include "serveraccess.h"
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>

namespace {

// Guaranteed to end with a /
QString constructUrl(QString const &baseUrl, QString const & /* bucket */)
{
    QString url;

    if (!baseUrl.contains(QLatin1String("://"))) {
        url += QStringLiteral("https://");
    }

    url += baseUrl;
    if (!url.endsWith('/')) {
        url += '/';
    }

    /*
    url += bucket;

    if (!url.endsWith('/')) {
        url += '/';
    }
    */

    return url;
}

QString constructListUrl(QString const &baseUrl, QString const &bucket, QString const &prefix)
{
    QString url = constructUrl(baseUrl, bucket);
    url += prefix;

    return url;
}

QString constructFileUrl(QString const &baseUrl, QString const &bucket, QString const &key)
{
    QString url = constructUrl(baseUrl, bucket);
    url += key;

    return url;
}

} // Empty namespace

ServerResult::ServerResult(QNetworkReply *reply, QObject *parent) : QObject(parent)
  , m_reply(reply)
{
    connect(reply, &QNetworkReply::finished, this, &ServerResult::onFinished);
}

ServerListResult::ServerListResult(QNetworkReply *reply, QString const &prefix, QObject *parent)
    : ServerResult (reply, parent)
    , m_prefix(prefix)
{
}

ServerGetResult::ServerGetResult(QNetworkReply *reply, QObject *parent) : ServerResult (reply, parent)
{
}

ServerGetFileResult::ServerGetFileResult(QNetworkReply *reply, QString const &filename, QObject *parent) : ServerResult (reply, parent)
  , m_filename(filename)
{
}

ServerResult::~ServerResult()
{
    m_reply->deleteLater();
}

QNetworkReply::NetworkError ServerResult::error() const
{
    return m_reply->error();
}

ServerAccess::ServerAccess(QObject *parent) : QObject(parent)
  , m_manager(new QNetworkAccessManager(this))
{
}

ServerAccess::~ServerAccess()
{
}

void ServerAccess::initialise()
{
}

void ServerAccess::setId(QString const &id)
{
    if (m_id != id) {
        m_id = id;
        initialise();
        emit idChanged();
    }
}

void ServerAccess::setSecret(QString const &secret)
{
    if (m_secret != secret) {
        m_secret = secret;
        initialise();
        emit secretChanged();
    }
}

void ServerAccess::setBaseUrl(QString const &baseUrl)
{
    if (m_baseUrl != baseUrl) {
        m_baseUrl = baseUrl;
        initialise();
        emit baseUrlChanged();
    }
}

void ServerAccess::setBucket(QString const &bucket)
{
    if (m_bucket != bucket) {
        m_bucket = bucket;
        emit bucketChanged();
    }
}

QString ServerAccess::id() const
{
    return m_id;
}

QString ServerAccess::secret() const
{
    return m_secret;
}

QString ServerAccess::baseUrl() const
{
    return m_baseUrl;
}

QString ServerAccess::bucket() const
{
    return m_bucket;
}

ServerListResult *ServerAccess::list(QString const &prefix)
{
    QString url;
    QNetworkReply *reply;
    ServerListResult *result;

    url = constructListUrl(m_baseUrl, m_bucket, prefix);

    reply = performOp(GET, url, nullptr, nullptr);
    result = new ServerListResult(reply, prefix, this);

    return result;
}

QNetworkReply *ServerAccess::performOp(Method method, QString const &url, QIODevice *in, const char *content_type)
{
    QNetworkRequest request;
    QNetworkReply *reply;

    request.setUrl(QUrl(QString(url)));

    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    qDebug() << "Request to: " << url;

    switch (method) {
    case DELETE:
        qDebug() << "DELETE request";
        reply = m_manager->deleteResource(request);
        break;
    case PUT:
        qDebug() << "PUT request";
        if (content_type) {
            request.setHeader(QNetworkRequest::ContentTypeHeader, content_type);
        }
        reply = m_manager->put(request, in);
        break;
    default: // GET
        qDebug() << "GET request";
        reply = m_manager->get(request);
        break;
    }

    return reply;
}

void ServerListResult::onFinished()
{
    qDebug() << "ServerListResult::onFinished";

    QByteArray data;
    QJsonParseError error;
    QJsonDocument doc;
    bool result = true;

    if (m_reply->error() == QNetworkReply::NoError) {
        // Should return a json array
        data = m_reply->readAll();
        qDebug() << "Received list: " << data;
    }
    else {
        qDebug() << "Error: " << m_reply->error();
        result = false;
    }

    if (result) {
        doc = QJsonDocument::fromJson(data, &error);
        if (doc.isNull()) {
            qDebug() << "JSON error: " << error.errorString();
            result = false;
        }
    }

    if (result && !doc.isArray()) {
        qDebug() << "JSON error: expected an array";
        result = false;
    }

    if (result) {
        m_keys.clear();
        for (QJsonValue value : doc.array()) {
            QString converted;
            switch (value.type()) {
            case QJsonValue::Double:
                converted = QString::number(value.toInt());
                break;
            case QJsonValue::String:
                converted = value.toString();
                break;
            default:
                // Don't append anything
                qDebug() << "JSON error: converting type " << value.type() << " not supported";
                break;
            }
            if (!converted.isEmpty()) {
                m_keys.append(m_prefix + "/" + converted);
            }
        }
        emit keysChanged();
        qDebug() << "Successfullly parsed list: " << m_keys;
    }

    emit finished();
}

QStringList ServerListResult::keys() const
{
    return m_keys;
}

void ServerGetResult::onFinished()
{
    m_data = m_reply->readAll();

    emit dataChanged();
}

QByteArray ServerGetResult::data() const
{
    return m_data;
}

ServerGetFileResult *ServerAccess::getFile(QString const &key, QString const &filename)
{
    QString url;
    QNetworkReply *reply;
    ServerGetFileResult *result;

    url = constructFileUrl(m_baseUrl, m_bucket, key);

    reply = performOp(GET, url, nullptr, nullptr);
    result = new ServerGetFileResult(reply, filename, this);

    return result;
}

void ServerResult::onFinished()
{
    qDebug() << "ServerResult::onFinished";

    if (m_reply->error() == QNetworkReply::NoError) {
        qDebug() << "Download complete";
    }
    else {
        qDebug() << "Error: " << m_reply->error();
    }

    emit finished();
}

void ServerGetFileResult::onFinished()
{
    qDebug() << "ServerResult::onFinished";

    if (m_reply->error() == QNetworkReply::NoError) {
        QFile file(m_filename);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_reply->readAll());
            qDebug() << "Data written to file: " << m_filename;
            qDebug() << "Data size: " << file.pos();
            file.close();
        }
        else {
            qDebug() << "Error writing to file: " << m_filename;
        }
    }
    else {
        qDebug() << "Error: " << m_reply->error();
    }

    emit finished();
}
