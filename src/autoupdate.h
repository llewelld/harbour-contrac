#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#include <QObject>
#include <QTime>
#include <QtDBus>

class BackgroundActivity;
class AppSettings;

class AutoUpdate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(qint32 updateTime READ updateTime WRITE setUpdateTime NOTIFY updateTimeChanged)

public:
    explicit AutoUpdate(QObject *parent = nullptr);
    ~AutoUpdate();

    bool running() const;
    bool autoUpdate() const;
    qint32 updateTime() const;
    void setRunning(bool running);
    void setAutoUpdate(bool autoUpdate);
    void setUpdateTime(qint32 updateTime);

public slots:
    void updateComplete();

signals:
    void startUpdate();
    void runningChanged();
    void autoUpdateChanged();
    void updateTimeChanged();

private slots:
    void stateChanged();
    void autoUpdateSettingChanged();
    void autoUpdateTimeSettingChanged();

private:
    void configureNextUpdate();

private:
    BackgroundActivity *m_backgroundActivity;
    bool m_running;
    AppSettings &m_settings;
};

#endif // AUTOUPDATE_H
