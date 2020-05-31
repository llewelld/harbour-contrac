#ifndef DBUSPROXY_H
#define DBUSPROXY_H

#include <QObject>
#include <QDBusPendingCall>

class QDBusInterface;

#define SERVICE_NAME "uk.co.flypig.contrac"

class DBusProxy : public QObject
{
    Q_OBJECT

    // Non-standard additions
    Q_PROPERTY(quint32 receivedCount READ receivedCount NOTIFY receivedCountChanged)
    Q_PROPERTY(quint32 sentCount READ sentCount NOTIFY sentCountChanged)
public:
    explicit DBusProxy(QObject *parent = nullptr);
    ~DBusProxy();

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    // Non-standard additions
    quint32 receivedCount() const;
    quint32 sentCount() const;

signals:

    // Non-standard additions
    void receivedCountChanged();
    void sentCountChanged();

public slots:
    void onPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated);
    void onReceiveCountChanged();
    void onSentCountChanged();

private:
    QDBusInterface *m_interface;
    quint32 m_receivedCount;
    quint32 m_sentCount;
};

#endif // DBUSPROXY_H
