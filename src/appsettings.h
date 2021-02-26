#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDate>
#include <QObject>
#include <QSettings>

#include "../contracd/src/exposuresummary.h"
#include "notifications.h"
#include "riskscoreclass.h"

class QQmlEngine;
class QJSEngine;

class AppSettings : public QObject
{
    Q_OBJECT

    // General properties
    Q_PROPERTY(QString downloadServer READ downloadServer WRITE setDownloadServer NOTIFY downloadServerChanged)
    Q_PROPERTY(QString uploadServer READ uploadServer WRITE setUploadServer NOTIFY uploadServerChanged)
    Q_PROPERTY(QString verificationServer READ verificationServer WRITE setVerificationServer NOTIFY verificationServerChanged)
    Q_PROPERTY(ExposureSummary *latestSummary READ latestSummary WRITE setLatestSummary NOTIFY latestSummaryChanged)
    Q_PROPERTY(QDateTime summaryUpdated READ summaryUpdated WRITE setSummaryUpdated NOTIFY summaryUpdatedChanged)
    Q_PROPERTY(quint32 infoViewed READ infoViewed WRITE setInfoViewed NOTIFY infoViewedChanged)
    Q_PROPERTY(QString countryCode READ countryCode WRITE setCountryCode NOTIFY countryCodeChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(qint32 autoUpdateTime READ autoUpdateTime WRITE setAutoUpdateTime NOTIFY autoUpdateTimeChanged)
    Q_PROPERTY(Notifications::NotifyLevel notifyLevel READ notifyLevel WRITE setNotifyLevel NOTIFY notifyLevelChanged)
    Q_PROPERTY(QList<quint32> notifyIds READ notifyIds WRITE setNotifyIds NOTIFY notifyIdsChanged)

    // Attenuation duration properties
    Q_PROPERTY(QList<double> riskWeights READ riskWeights WRITE setRiskWeights NOTIFY riskWeightsChanged)
    Q_PROPERTY(qint32 defaultBuckeOffset READ defaultBuckeOffset WRITE setDefaultBuckeOffset NOTIFY defaultBuckeOffsetChanged)
    Q_PROPERTY(qint32 normalizationDivisor READ normalizationDivisor WRITE setNormalizationDivisor NOTIFY normalizationDivisorChanged)
    Q_PROPERTY(QList<RiskScoreClass> riskScoreClasses READ riskScoreClasses WRITE setRiskScoreClasses NOTIFY riskScoreClassesChanged)

    Q_PROPERTY(QDate regTokenReceived READ regTokenReceived WRITE setRegTokenReceived NOTIFY regTokenReceivedChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);
    ~AppSettings();

    static void instantiate(QObject *parent = nullptr);
    static AppSettings &getInstance();
    static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine);

    // Constant image properties
    Q_INVOKABLE QString getImageDir() const;
    Q_INVOKABLE QString getImageUrl(QString const &id) const;

    // Get general properties
    QString downloadServer() const;
    QString uploadServer() const;
    QString verificationServer() const;
    ExposureSummary *latestSummary();
    QDateTime summaryUpdated() const;
    quint32 infoViewed() const;
    QString countryCode() const;
    bool autoUpdate() const;
    qint32 autoUpdateTime() const;
    QDate regTokenReceived() const;
    Notifications::NotifyLevel notifyLevel() const;
    QList<quint32> notifyIds() const;

    // Get attenuation duration properties
    QList<double> riskWeights() const;
    qint32 defaultBuckeOffset() const;
    qint32 normalizationDivisor() const;
    QList<RiskScoreClass> riskScoreClasses() const;

    // Set general properties
    void setDownloadServer(QString const &downloadServer);
    void setUploadServer(QString const &uploadServer);
    void setVerificationServer(QString const &verificationServer);
    void setLatestSummary(ExposureSummary const *latestSummary);
    void setSummaryUpdated(QDateTime summaryUpdated);
    void setInfoViewed(quint32 infoViewed);
    void setCountryCode(QString countryCode);
    void setAutoUpdate(bool autoUpdate);
    void setAutoUpdateTime(qint32 autoUpdateTime);
    void setRegTokenReceived(const QDate &receivedDate);
    void setNotifyLevel(Notifications::NotifyLevel notifyLevel);
    void setNotifyIds(QList<quint32> notifyIds);

    // Set attenuation duration properties
    void setRiskWeights(QList<double> riskWeights);
    void setDefaultBuckeOffset(qint32 defaultBuckeOffset);
    void setNormalizationDivisor(qint32 normalizationDivisor);
    void setRiskScoreClasses(QList<RiskScoreClass> riskScoreClasses);

signals:
    // General property signals
    void downloadServerChanged();
    void uploadServerChanged();
    void verificationServerChanged();
    void latestSummaryChanged();
    void summaryUpdatedChanged();
    void infoViewedChanged();
    void countryCodeChanged();
    void autoUpdateChanged();
    void autoUpdateTimeChanged();
    void notifyLevelChanged();
    void notifyIdsChanged();

    // Attenuation duration property signals
    void riskWeightsChanged();
    void defaultBuckeOffsetChanged();
    void normalizationDivisorChanged();
    void riskScoreClassesChanged();
    void regTokenReceivedChanged();

private:
    bool upgrade();
    bool upgradeToVersion1();

private:
    static AppSettings *instance;
    QSettings m_settings;

    // General properties
    QString m_imageDir;
    QString m_downloadServer;
    QString m_uploadServer;
    QString m_verificationServer;
    ExposureSummary m_latestSummary;
    QDateTime m_summaryUpdated;
    quint32 m_infoViewed;
    QString m_countryCode;
    bool m_autoUpdate;
    qint32 m_autoUpdateTime;
    Notifications::NotifyLevel m_notifyLevel;
    QList<quint32> m_notifyIds;

    // Attenuation duration properties
    QList<double> m_riskWeights;
    qint32 m_defaultBuckeOffset;
    qint32 m_normalizationDivisor;
    QList<RiskScoreClass> m_riskScoreClasses;
    QDate m_regTokenReceived;
};

QDataStream &operator<<(QDataStream &out, const QList<quint32> &list);
QDataStream &operator>>(QDataStream &in, QList<quint32> &list);

#endif // APPSETTINGS_H
