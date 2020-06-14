extern "C" {
#include "s3/s3internal.h"
}

#include "s3access.h"

S3Result::S3Result(QNetworkReply *reply, QObject *parent) : QObject(parent)
  , m_reply(reply)
{
    connect(reply, &QNetworkReply::finished, this, &S3Result::finished);
}

S3ListResult::S3ListResult(QNetworkReply *reply, QObject *parent) : S3Result (reply, parent)
{
    connect(reply, &QNetworkReply::finished, this, &S3ListResult::onFinished);
}

S3ListResult::~S3ListResult()
{
    disconnect(m_reply, &QNetworkReply::finished, this, &S3ListResult::onFinished);
}

S3GetResult::S3GetResult(QNetworkReply *reply, QObject *parent) : S3Result (reply, parent)
{
    connect(reply, &QNetworkReply::finished, this, &S3GetResult::onFinished);
}

S3GetResult::~S3GetResult()
{
    disconnect(m_reply, &QNetworkReply::finished, this, &S3GetResult::onFinished);
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
    char *date;
    char *sign_data;
    char *url;
    QNetworkReply *reply;
    S3ListResult *result;

    date = s3_make_date();

    asprintf(&sign_data, "%s\n\n\n%s\n/%s/", "GET", date, m_bucket.toLatin1().data());
    asprintf(&url, "http://%s.%s/?delimiter=/%s%s", m_bucket.toLatin1().data(), m_baseUrl.toLatin1().data(), !prefix.isEmpty() ? "&prefix=" : "", !prefix.isEmpty() ? prefix.toLatin1().data() : "");

    reply = performOp(GET, url, sign_data, date, nullptr, nullptr, nullptr);

    result = new S3ListResult(reply, this);

    free(url);
    free(sign_data);
    free(date);

    return result;
}

QNetworkReply *S3Access::performOp(Method method, const char *url, const char *sign_data, const char *date, QIODevice *in, const char *content_md5, const char *content_type)
{
    QNetworkRequest request;
    char *digest;
    QNetworkReply *reply;

    request.setUrl(QUrl(QString(url)));
    digest = s3_hmac_sign(m_secret.toLatin1().data(), sign_data, strlen(sign_data));

    request.setRawHeader("Date", date);
    request.setRawHeader("Authorization", QString(QStringLiteral("AWS %1:%2")).arg(m_id).arg(digest).toLocal8Bit());
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    switch (method) {
    case DELETE:
        reply = m_manager->deleteResource(request);
        break;
    case PUT:
        if (content_type) {
            request.setHeader(QNetworkRequest::ContentTypeHeader, content_type);
        }

        if (content_md5) {
            request.setRawHeader("Content-MD5", content_md5);
        }

        reply = m_manager->put(request, in);
        break;
    default: // GET
        reply = m_manager->get(request);
        break;
    }

    free(digest);

    return reply;
}

void S3ListResult::onFinished()
{
    QByteArray data;
    struct s3_bucket_entry_head *entries;
    struct s3_bucket_entry *e;

    data = m_reply->readAll();

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

