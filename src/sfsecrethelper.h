#ifndef SFSECRETHELPER_H
#define SFSECRETHELPER_H

#include <QObject>
#include <QtCore/QByteArray>
#include <QtCore/QObject>

#include <Secrets/secret.h>
#include <Secrets/secretmanager.h>

class QQmlEngine;
class QJSEngine;

class SFSecretHelper : public QObject
{
    Q_OBJECT
public:
    struct Response
    {
        int returncode;
        QByteArray data;
    };

    explicit SFSecretHelper(QString collection, QObject *parent = nullptr);

    static void instantiate(const QString &collection, QObject *parent = nullptr);
    static SFSecretHelper &getInstance();
    static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine);

    bool checkforCollectionExistance();
    bool createCollection();

    Response store(const QString &key, const QByteArray &value, bool rerun=true);
    Response get(const QString &key);
    Response remove(const QString &key);

private:
    Sailfish::Secrets::Secret::Identifier generateIdentifier(const QString &name);

private:
    static SFSecretHelper *instance;

    Sailfish::Secrets::SecretManager m_secretManager;
    QString m_collectionName;
};

#endif // SFSECRETHELPER_H
