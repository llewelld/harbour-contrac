#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QQmlEngine>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT

    // General properties
    Q_PROPERTY(QString downloadServer READ downloadServer WRITE setDownloadServer NOTIFY downloadServerChanged)
    Q_PROPERTY(QString uploadServer READ uploadServer WRITE setUploadServer NOTIFY uploadServerChanged)
    Q_PROPERTY(QString verificationServer READ verificationServer WRITE setVerificationServer NOTIFY verificationServerChanged)

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    static void instantiate(QObject *parent = nullptr);
    static Settings & getInstance();
    static QObject * provider(QQmlEngine *engine, QJSEngine *scriptEngine);


    Q_INVOKABLE QString downloadServer() const;
    Q_INVOKABLE QString uploadServer() const;
    Q_INVOKABLE QString verificationServer() const;

signals:
    void downloadServerChanged();
    void uploadServerChanged();
    void verificationServerChanged();

public slots:
    void setDownloadServer(QString const &downloadServer);
    void setUploadServer(QString const &uploadServer);
    void setVerificationServer(QString const &verificationServer);

private:
    static Settings * instance;
    QSettings settings;

    QString m_downloadServer;
    QString m_uploadServer;
    QString m_verificationServer;
};

#endif // SETTINGS_H
