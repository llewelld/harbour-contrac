#include <Secrets/collectionnamesrequest.h>
#include <Secrets/createcollectionrequest.h>
#include <Secrets/deletesecretrequest.h>
#include <Secrets/interactionparameters.h>
#include <Secrets/result.h>
#include <Secrets/storedsecretrequest.h>
#include <Secrets/storesecretrequest.h>
#include <QDebug>

#include "sfsecrethelper.h"

using namespace Sailfish::Secrets;

SFSecretHelper *SFSecretHelper::instance = nullptr;

SFSecretHelper::SFSecretHelper(QString collection, QObject *parent)
    : QObject{parent}
    , m_collectionName{collection}
{
    if (!this->checkforCollectionExistance())
        this->createCollection();
}

void SFSecretHelper::instantiate(const QString &collection, QObject *parent)
{
    if (instance == nullptr) {
        instance = new SFSecretHelper(collection, parent);
    }
}

SFSecretHelper &SFSecretHelper::getInstance()
{
    return *instance;
}

QObject *SFSecretHelper::provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return instance;
}

bool SFSecretHelper::checkforCollectionExistance()
{
    Secret::Identifier const identifier = this->generateIdentifier(QString());

    CollectionNamesRequest request;
    request.setManager(&m_secretManager);
    request.setStoragePluginName(identifier.storagePluginName());
    request.startRequest();
    request.waitForFinished();
    qDebug() << "Start check for existing collections.";
    for (QString name : request.collectionNames()) {
        qDebug() << "Found Collection: " << name;
        if (name == this->m_collectionName)
            return true;
    }
    qDebug() << "Collection " << this->m_collectionName << " not found!";
    return false;
}

bool SFSecretHelper::createCollection()
{
    Secret::Identifier const identifier = this->generateIdentifier(QString());

    qDebug() << "Create collection " << identifier.collectionName();

    CreateCollectionRequest request;
    request.setManager(&m_secretManager);
    request.setCollectionLockType(CreateCollectionRequest::DeviceLock);
    request.setDeviceLockUnlockSemantic(SecretManager::DeviceLockKeepUnlocked);
    request.setAccessControlMode(SecretManager::OwnerOnlyMode);
    request.setUserInteractionMode(SecretManager::SystemInteraction);
    request.setCollectionName(identifier.collectionName());
    request.setStoragePluginName(identifier.storagePluginName());
    request.setEncryptionPluginName(identifier.storagePluginName());
    request.startRequest();
    request.waitForFinished();

    if (request.result().code() != Sailfish::Secrets::Result::Succeeded) {
        qWarning() << "Failed to create collection " << m_collectionName << " for storing the key!";
        qWarning() << "Error:" << request.result().errorCode() << request.result().errorMessage();
        return false;
    }
    return true;
}

SFSecretHelper::Response SFSecretHelper::store(const QString &key, const QByteArray &value, bool rerun)
{
    Secret::Identifier const identifier = this->generateIdentifier(key);

    Secret secret(identifier);
    secret.setData(value);

    StoreSecretRequest storeRequest;
    storeRequest.setManager(&m_secretManager);
    storeRequest.setSecretStorageType(StoreSecretRequest::CollectionSecret);
    storeRequest.setUserInteractionMode(SecretManager::SystemInteraction);
    storeRequest.setSecret(secret);
    storeRequest.startRequest();
    storeRequest.waitForFinished();

    if (storeRequest.result().code() != Sailfish::Secrets::Result::Succeeded) {
        if (storeRequest.result().errorCode() == Sailfish::Secrets::Result::DatabaseQueryError && rerun) {
            this->remove(key);
            return this->store(key, value, false);
        }
        qWarning() << "Failed to store key: " << key << " !";
        qWarning() << "Error:" << storeRequest.result().errorCode() << storeRequest.result().errorMessage();
        return Response{1, value};
    }
    //Success
    return Response{0, nullptr};
}

SFSecretHelper::Response SFSecretHelper::get(const QString &key)
{
    Secret::Identifier const identifier = this->generateIdentifier(key);

    StoredSecretRequest request;
    request.setManager(&m_secretManager);
    request.setUserInteractionMode(Sailfish::Secrets::SecretManager::SystemInteraction);
    request.setIdentifier(identifier);
    request.startRequest();
    request.waitForFinished();

    if (request.result().code() != Sailfish::Secrets::Result::Succeeded) {
        qWarning() << "Failed to retrieve key: " << key << " !";
        qWarning() << "Error:" << request.result().errorCode() << request.result().errorMessage();
        return Response{1, nullptr};
    }
    //Success
    return Response{0, request.secret().data()};
}

SFSecretHelper::Response SFSecretHelper::remove(const QString &key)
{
    Secret::Identifier const identifier = this->generateIdentifier(key);

    Secret secret(identifier);

    DeleteSecretRequest request;
    request.setManager(&m_secretManager);
    request.setUserInteractionMode(Sailfish::Secrets::SecretManager::SystemInteraction);
    request.setIdentifier(identifier);
    request.startRequest();
    request.waitForFinished();

    if (request.result().code() != Sailfish::Secrets::Result::Succeeded) {
        qWarning() << "Failed to delete key: " << key << " !";
        qWarning() << "Error:" << request.result().errorCode() << request.result().errorMessage();
        return Response{1, nullptr};
    }
    //Success
    return Response{0, nullptr};
}

Secret::Identifier SFSecretHelper::generateIdentifier(const QString &name)
{
    return Secret::Identifier(name, this->m_collectionName, SecretManager::DefaultEncryptedStoragePluginName);
}
