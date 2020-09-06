#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QDate>

#include "../contracd/src/exposuresummary.h"

class Settings : public QObject
{
    Q_OBJECT

    // General properties
    Q_PROPERTY(QString downloadServer READ downloadServer WRITE setDownloadServer NOTIFY downloadServerChanged)
    Q_PROPERTY(QString uploadServer READ uploadServer WRITE setUploadServer NOTIFY uploadServerChanged)
    Q_PROPERTY(QString verificationServer READ verificationServer WRITE setVerificationServer NOTIFY verificationServerChanged)
    Q_PROPERTY(ExposureSummary *latestSummary READ latestSummary WRITE setLatestSummary NOTIFY latestSummaryChanged)
    Q_PROPERTY(QDateTime summaryUpdated READ summaryUpdated WRITE setSummaryUpdated NOTIFY summaryUpdatedChanged)

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

signals:
    void downloadServerChanged();
    void uploadServerChanged();
    void verificationServerChanged();
    void latestSummaryChanged();
    void summaryUpdatedChanged();

public slots:
    void setDownloadServer(QString const &downloadServer);
    void setUploadServer(QString const &uploadServer);
    void setVerificationServer(QString const &verificationServer);
    void setLatestSummary(ExposureSummary const *latestSummary);
    void setSummaryUpdated(QDateTime summaryUpdated);

private:
    static Settings * instance;
    QSettings settings;

    QString m_downloadServer;
    QString m_uploadServer;
    QString m_verificationServer;
    ExposureSummary m_latestSummary;
    QDateTime m_summaryUpdated;
};

#endif // SETTINGS_H
