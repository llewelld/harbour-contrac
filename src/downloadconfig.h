#ifndef DOWNLOADCONFIG_H
#define DOWNLOADCONFIG_H

#include <QObject>

#include "riskscoreclass.h"

class ServerAccess;
class ExposureConfiguration;

namespace diagnosis {
class ApplicationConfiguration;
}

class DownloadConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(ErrorType error READ error NOTIFY errorChanged)
    Q_PROPERTY(ExposureConfiguration config READ config NOTIFY configChanged)

public:
    enum Status
    {
        StatusIdle,
        StatusDownloading,
        StatusError
    };
    enum ErrorType
    {
        ErrorNone,
        ErrorNetwork
    };
    explicit DownloadConfig(QObject *parent = nullptr);

    Q_INVOKABLE void downloadLatest();

    bool downloading() const;
    Status status() const;
    ErrorType error() const;
    ExposureConfiguration *config() const;

signals:
    void downloadingChanged();
    void downloadComplete(QString const &filename);
    void statusChanged();
    void errorChanged();
    void configChanged();
    void attenuationDurationConfigChanged();

private slots:
    void setStatus(Status status);

private:
    void finalise();
    void setStatusError(ErrorType error);
    bool loadConfig();
    void applyConfiguration(diagnosis::ApplicationConfiguration const &appConfig);

private:
    ServerAccess *m_serverAccess;
    bool m_downloading;
    Status m_status;
    ErrorType m_error;
    ExposureConfiguration *m_configuration;
};

#endif // DOWNLOADCONFIG_H
