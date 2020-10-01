#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDate>

#include "riskscoreclass.h"
#include "../contracd/src/exposuresummary.h"

class QQmlEngine;
class QJSEngine;

class Settings : public QObject
{
    Q_OBJECT

    // General properties
    Q_PROPERTY(QString downloadServer READ downloadServer WRITE setDownloadServer NOTIFY downloadServerChanged)
    Q_PROPERTY(QString uploadServer READ uploadServer WRITE setUploadServer NOTIFY uploadServerChanged)
    Q_PROPERTY(QString verificationServer READ verificationServer WRITE setVerificationServer NOTIFY verificationServerChanged)
    Q_PROPERTY(ExposureSummary *latestSummary READ latestSummary WRITE setLatestSummary NOTIFY latestSummaryChanged)
    Q_PROPERTY(QDateTime summaryUpdated READ summaryUpdated WRITE setSummaryUpdated NOTIFY summaryUpdatedChanged)
    Q_PROPERTY(quint32 infoViewed READ infoViewed WRITE setInfoViewed NOTIFY infoViewedChanged)

    // Attenuation duration properties
    Q_PROPERTY(QList<double> riskWeights READ riskWeights WRITE setRiskWeights NOTIFY riskWeightsChanged)
    Q_PROPERTY(qint32 defaultBuckeOffset READ defaultBuckeOffset WRITE setDefaultBuckeOffset NOTIFY defaultBuckeOffsetChanged)
    Q_PROPERTY(qint32 normalizationDivisor READ normalizationDivisor WRITE setNormalizationDivisor NOTIFY normalizationDivisorChanged)
    Q_PROPERTY(QList<RiskScoreClass> riskScoreClasses READ riskScoreClasses WRITE setRiskScoreClasses NOTIFY riskScoreClassesChanged)

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    static void instantiate(QObject *parent = nullptr);
    static Settings & getInstance();
    static QObject * provider(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE QString downloadServer() const;
    Q_INVOKABLE QString uploadServer() const;
    Q_INVOKABLE QString verificationServer() const;
    Q_INVOKABLE ExposureSummary *latestSummary();
    Q_INVOKABLE QDateTime summaryUpdated() const;
    Q_INVOKABLE quint32 infoViewed() const;

    Q_INVOKABLE QList<double> riskWeights() const;
    Q_INVOKABLE qint32 defaultBuckeOffset() const;
    Q_INVOKABLE qint32 normalizationDivisor() const;
    Q_INVOKABLE QList<RiskScoreClass> riskScoreClasses() const;

    Q_INVOKABLE QString getImageDir() const;
    Q_INVOKABLE QString getImageUrl(QString const &id) const;

signals:
    void downloadServerChanged();
    void uploadServerChanged();
    void verificationServerChanged();
    void latestSummaryChanged();
    void summaryUpdatedChanged();
    void infoViewedChanged();
    void riskWeightsChanged();
    void defaultBuckeOffsetChanged();
    void normalizationDivisorChanged();
    void riskScoreClassesChanged();

public slots:
    void setDownloadServer(QString const &downloadServer);
    void setUploadServer(QString const &uploadServer);
    void setVerificationServer(QString const &verificationServer);
    void setLatestSummary(ExposureSummary const *latestSummary);
    void setSummaryUpdated(QDateTime summaryUpdated);
    void setInfoViewed(quint32 infoViewed);
    void setRiskWeights(QList<double> riskWeights);
    void setDefaultBuckeOffset(qint32 defaultBuckeOffset);
    void setNormalizationDivisor(qint32 normalizationDivisor);
    void setRiskScoreClasses(QList<RiskScoreClass> riskScoreClasses);

private:
    bool upgrade();
    bool upgradeToVersion1();

private:
    static Settings * instance;
    QSettings m_settings;

    QString m_downloadServer;
    QString m_uploadServer;
    QString m_verificationServer;
    ExposureSummary m_latestSummary;
    QDateTime m_summaryUpdated;
    quint32 m_infoViewed;
    QString m_imageDir;
    QList<double> m_riskWeights;
    qint32 m_defaultBuckeOffset;
    qint32 m_normalizationDivisor;
    QList<RiskScoreClass> m_riskScoreClasses;
};

#endif // SETTINGS_H
