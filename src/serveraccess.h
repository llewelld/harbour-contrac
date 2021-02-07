#ifndef SERVERACCESS_H
#define SERVERACCESS_H

#include <QNetworkReply>
#include <QObject>

class ServerResult : public QObject
{
    Q_OBJECT

public:
    explicit ServerResult(QNetworkReply *reply, QObject *parent = nullptr);
    ~ServerResult();

    virtual QNetworkReply::NetworkError error() const;

signals:
    void finished();
    void progress(qint64 bytesReceived, qint64 bytesTotal);

protected slots:
    virtual void onFinished();

protected:
    QNetworkReply *m_reply;
};

class ServerListResult : public ServerResult
{
    Q_OBJECT
    Q_PROPERTY(QStringList keys READ keys NOTIFY keysChanged)

public:
    explicit ServerListResult(QNetworkReply *reply, QString const &prefix, QObject *parent = nullptr);

    QStringList keys() const;

signals:
    void keysChanged();

private slots:
    virtual void onFinished();

private:
    QString m_prefix;
    QStringList m_keys;
};

class ServerGetResult : public ServerResult
{
    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data NOTIFY dataChanged)

public:
    explicit ServerGetResult(QNetworkReply *reply, QObject *parent = nullptr);

    QByteArray data() const;

signals:
    void dataChanged();

private slots:
    virtual void onFinished();

private:
    QByteArray m_data;
};

class ServerGetFileResult : public ServerResult
{
    Q_OBJECT

public:
    explicit ServerGetFileResult(QNetworkReply *reply, QString const &filename, QObject *parent = nullptr);

private slots:
    virtual void onFinished();

private:
    QString m_filename;
};

class ServerAccess : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString secret READ secret WRITE setSecret NOTIFY secretChanged)
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(QString bucket READ bucket WRITE setBucket NOTIFY bucketChanged)

public:
    explicit ServerAccess(QObject *parent = nullptr);
    ~ServerAccess();

    ServerListResult *list(QString const &prefix);
    ServerGetResult *get(QString const &key);
    ServerGetFileResult *getFile(QString const &key, QString const &filename);
    ServerResult *put(QString const &key, QString const &contentType, QByteArray const &data);
    ServerResult *putFile(QString const &key, QString const &contentType, QString const &filename);
    ServerResult *remove(QString const &key);

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
    enum Method
    {
        GET,
        PUT,
        DELETE
    };
    void initialise();
    QNetworkReply *performOp(Method method, QString const &url, QIODevice *in, const char *content_type);

private:
    QString m_id;
    QString m_secret;
    QString m_baseUrl;
    QString m_bucket;

    QNetworkAccessManager *m_manager;
};

#endif // SERVERACCESS_H
