#ifndef TEMPORARYEXPOSUREKEY_H
#define TEMPORARYEXPOSUREKEY_H

#include <QObject>
#include <QDBusArgument>

class TemporaryExposureKey : public QObject
{
    Q_OBJECT
    Q_ENUMS(RiskLevel)
    Q_PROPERTY(QByteArray keyData READ keyData WRITE setKeyData NOTIFY keyDataChanged)
    Q_PROPERTY(quint32 rollingStartNumber READ rollingStartNumber WRITE setRollingStartNumber NOTIFY rollingStartNumberChanged)
    Q_PROPERTY(quint32 rollingPeriod READ rollingPeriod NOTIFY rollingPeriodChanged)
    Q_PROPERTY(quint8 transmissionRiskLevel READ transmissionRiskLevel WRITE setTransmissionRiskLevel NOTIFY transmissionRiskLevelChanged)

public:
    explicit TemporaryExposureKey(QObject *parent = nullptr);
    TemporaryExposureKey(TemporaryExposureKey const& temporaryExopsureKey);
    TemporaryExposureKey& operator=( const TemporaryExposureKey &other);

    enum RiskLevel
    {
        RiskLevelInvalid = 0,
        RiskLevelLowest,
        RIskLevelLow,
        RiskLevelLowMedium,
        RiskLevelMedium,
        RiskLevelMediumHigh,
        RiskLevelHigh,
        RiskLevelVeryHigh,
        RiskLevelHighest
    };

    QByteArray keyData() const;
    quint32 rollingStartNumber() const;
    quint32 rollingPeriod() const;
    quint8 transmissionRiskLevel() const;

    void setKeyData(QByteArray const &keyData);
    void setRollingStartNumber(quint32 rollingStartNumber);
    void setRollingPeriod(quint32 rollingPeriod);
    void setTransmissionRiskLevel(quint8 transmissionRiskLevel);

signals:
    void keyDataChanged();
    void rollingStartNumberChanged();
    void rollingPeriodChanged();
    void transmissionRiskLevelChanged();

public slots:

private:
    QByteArray m_keyData;
    quint32 m_rollingStartNumber;
    quint32 m_rollingPeriod;
    quint8 m_transmissionRiskLevel;
};

QDBusArgument &operator<<(QDBusArgument &argument, const TemporaryExposureKey &temporaryExposureKey);
QDBusArgument const &operator>>(const QDBusArgument &argument, TemporaryExposureKey &temporaryExposureKey);

Q_DECLARE_METATYPE(TemporaryExposureKey)

QDBusArgument &operator<<(QDBusArgument &argument, const QList<TemporaryExposureKey> &temporaryExposureKeyList);
QDBusArgument const &operator>>(const QDBusArgument &argument, QList<TemporaryExposureKey> &temporaryExposureKeyList);

Q_DECLARE_METATYPE(QList<TemporaryExposureKey>)

#endif // TEMPORARYEXPOSUREKEY_H
