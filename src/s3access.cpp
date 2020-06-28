extern "C" {
#include "s3/s3internal.h"
}

#include <QFile>

#include "s3access.h"

S3Result::S3Result(QNetworkReply *reply, QObject *parent) : QObject(parent)
  , m_reply(reply)
{
    connect(reply, &QNetworkReply::finished, this, &S3Result::onFinished);
}

S3ListResult::S3ListResult(QNetworkReply *reply, QObject *parent) : S3Result (reply, parent)
{
}

S3GetResult::S3GetResult(QNetworkReply *reply, QObject *parent) : S3Result (reply, parent)
{
}

S3GetFileResult::S3GetFileResult(QNetworkReply *reply, QString const &filename, QObject *parent) : S3Result (reply, parent)
  , m_filename(filename)
{
}

S3Result::~S3Result()
{
    m_reply->deleteLater();
}

S3Access::S3Access(QObject *parent) : QObject(parent)
  , m_s3(nullptr)
  , m_manager(new QNetworkAccessManager(this))
{
}

S3Access::~S3Access()
{
    if (m_s3) {
        s3_free(m_s3);
        m_s3 = nullptr;
    }
}

void S3Access::initialise()
{
    if (m_s3) {
        s3_free(m_s3);
        m_s3 = nullptr;
    }
    m_s3 = s3_init(m_id.toLatin1().data(), m_baseUrl.toLatin1().data(), m_baseUrl.toLatin1().data());
}

void S3Access::setId(QString const &id)
{
    if (m_id != id) {
        m_id = id;
        initialise();
        emit idChanged();
    }
}

void S3Access::setSecret(QString const &secret)
{
    if (m_secret != secret) {
        m_secret = secret;
        initialise();
        emit secretChanged();
    }
}

void S3Access::setBaseUrl(QString const &baseUrl)
{
    if (m_baseUrl != baseUrl) {
        m_baseUrl = baseUrl;
        initialise();
        emit baseUrlChanged();
    }
}

void S3Access::setBucket(QString const &bucket)
{
    if (m_bucket != bucket) {
        m_bucket = bucket;
        emit bucketChanged();
    }
}

QString S3Access::id() const
{
    return m_id;
}

QString S3Access::secret() const
{
    return m_secret;
}

QString S3Access::baseUrl() const
{
    return m_baseUrl;
}

QString S3Access::bucket() const
{
    return m_bucket;
}

S3ListResult *S3Access::list(QString const &prefix)
{
    QString signData;
    QString url;

    QNetworkReply *reply;
    S3ListResult *result;

    url = "http://" + m_baseUrl + "/" + m_bucket + "/?delimiter=/";
    if (!prefix.isEmpty()) {
        url += "&prefix=" + prefix;
    }

    reply = performOp(GET, url, nullptr, nullptr, nullptr);
    result = new S3ListResult(reply, this);

    return result;
}

QNetworkReply *S3Access::performOp(Method method, QString const &url, QIODevice *in, const char *content_md5, const char *content_type, QString signDataKey)
{
    QNetworkRequest request;
    char *digest;
    QString methodStr;
    QString date = QDateTime::currentDateTimeUtc().toString(Qt::RFC2822Date);
    QNetworkReply *reply;

    switch (method) {
    case DELETE:
        methodStr = "DELETE";
        qDebug() << "DELETE request";
        break;
    case PUT:
        methodStr = "PUT";
        qDebug() << "PUT request";
        break;
    default: // GET
        methodStr = "GET";
        qDebug() << "GET request";
        break;
    }

    QString signData = methodStr + "\n\n\n" + date  + "\n/" + m_bucket + "/";

    if (signDataKey != nullptr)
        signData+=signDataKey;

    request.setUrl(QUrl(QString(url)));
    digest = s3_hmac_sign(m_secret.toLatin1().data(), signData.toLatin1().data(), signData.toLatin1().size());

    request.setRawHeader("Date", date.toLocal8Bit());
    request.setRawHeader("Authorization", QString(QStringLiteral("AWS %1:%2")).arg(m_id).arg(digest).toLocal8Bit());
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

        if (content_md5) {
            request.setRawHeader("Content-MD5", content_md5);
        }

        reply = m_manager->put(request, in);
        break;
    default: // GET
        qDebug() << "GET request";
        reply = m_manager->get(request);
        break;
    }

    free(digest);

    return reply;
}

void S3ListResult::onFinished()
{
    qDebug() << "S3ListResult::onFinished";

    QByteArray data;
    struct s3_bucket_entry_head *entries;
    struct s3_bucket_entry *e;

    data = m_reply->readAll();
    if (m_reply->error() == QNetworkReply::NoError) {
        entries = s3_parse_bucket_response(data.data());

        m_keys.clear();
        if (entries != nullptr) {
            TAILQ_FOREACH(e, entries, list) {
                m_keys.append(QString(e->key));
            }
            s3_bucket_entries_free(entries);
        }
        emit keysChanged();
    }
    else {
        qDebug() << "Error: " << m_reply->error();
    }

    emit finished();
}

QStringList S3ListResult::keys() const
{
    return m_keys;
}

void S3GetResult::onFinished()
{
    m_data = m_reply->readAll();

    emit dataChanged();
}

QByteArray S3GetResult::data() const
{
    return m_data;
}

S3GetFileResult *S3Access::getFile(QString const &key, QString const &filename)
{
    QString signData;
    QString url;
    QNetworkReply *reply;
    S3GetFileResult *result;

    url = "http://" + m_baseUrl + "/" + m_bucket + "/" + key;

    reply = performOp(GET, url, nullptr, nullptr, nullptr, key);
    result = new S3GetFileResult(reply, filename, this);

    return result;
}

void S3Result::onFinished()
{
    qDebug() << "S3Result::onFinished";

    if (m_reply->error() == QNetworkReply::NoError) {
        qDebug() << "Download complete";
    }
    else {
        qDebug() << "Error: " << m_reply->error();
    }

    emit finished();
}

void S3GetFileResult::onFinished()
{
    qDebug() << "S3Result::onFinished";

    if (m_reply->error() == QNetworkReply::NoError) {
        QFile file(m_filename);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_reply->readAll());
            file.close();
            qDebug() << "Data written to file: " << m_filename;
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
