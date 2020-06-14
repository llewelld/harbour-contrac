#ifndef S3ACCESS_H
#define S3ACCESS_H

#include <QObject>
#include <QNetworkReply>

extern "C" {
#include "s3/s3.h"
}

class S3Result : public QObject
{
    Q_OBJECT

public:
    explicit S3Result(QNetworkReply *reply, QObject *parent = nullptr);
    ~S3Result();

signals:
    void finished();

protected:
    QNetworkReply *m_reply;
};

class S3ListResult : public S3Result
{
    Q_OBJECT
    Q_PROPERTY(QStringList keys READ keys NOTIFY keysChanged)

public:
    explicit S3ListResult(QNetworkReply *reply, QObject *parent = nullptr);
    ~S3ListResult();

    QStringList keys() const;

signals:
    void keysChanged();

private slots:
    void onFinished();

private:
    QStringList m_keys;
};

class S3GetResult : public S3Result
{
    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data NOTIFY dataChanged)

public:
    explicit S3GetResult(QNetworkReply *reply, QObject *parent = nullptr);
    ~S3GetResult();

    QByteArray data() const;

signals:
    void dataChanged();

private slots:
    void onFinished();

private:
    QByteArray m_data;
};

class S3Access : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString secret READ secret WRITE setSecret NOTIFY secretChanged)
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(QString bucket READ bucket WRITE setBucket NOTIFY bucketChanged)

public:
    explicit S3Access(QObject *parent = nullptr);
    ~S3Access();

    S3ListResult *list(QString const &prefix);
    S3GetResult *get(QString const &key);
    S3Result *get(QString const &key, QString const &filename);
    S3Result *put(QString const &key, QString const &contentType, QString const &filename);
    S3Result *put(QString const &key, QString const &contentType, QByteArray const &data);
    S3Result *remove(QString const &key);

    QString id() const;
    QString secret() const;
    QString baseUrl() const;
    QString bucket() const;

    void setId(QString const &id);
    void setSecret(QString const &secret);
    void setBaseUrl(QString const &baseUrl);
    void setBucket(QString const &bucket);

signals:
    void idChanged();
    void secretChanged();
    void baseUrlChanged();
    void bucketChanged();

public slots:

private:
    enum Method {
        GET,
        PUT,
        DELETE
    };
    void initialise();
    QNetworkReply *performOp(Method method, const char *url, const char *sign_data, const char *date, QIODevice *in, const char *content_md5, const char *content_type);

private:
    QString m_id;
    QString m_secret;
    QString m_baseUrl;
    QString m_bucket;

    struct S3 *m_s3;
    QNetworkAccessManager *m_manager;
};

#endif // S3ACCESS_H
