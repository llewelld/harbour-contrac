#ifndef EXPOSURESUMMARY_H
#define EXPOSURESUMMARY_H

#include <QObject>
#include <QDBusArgument>

class ExposureSummary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 daysSinceLastExposure READ daysSinceLastExposure WRITE setDaysSinceLastExposure NOTIFY daysSinceLastExposureChanged)
    Q_PROPERTY(quint32 matchedKeyCount READ matchedKeyCount WRITE setMatchedKeyCount NOTIFY matchedKeyCountChanged)
    Q_PROPERTY(qint32 maximumRiskScore READ maximumRiskScore WRITE setMaximumRiskScore NOTIFY maximumRiskScoreChanged)
    Q_PROPERTY(QList<qint32> attenuationDurations READ attenuationDurations WRITE setAttenuationDurations NOTIFY attenuationDurationsChanged)
    Q_PROPERTY(qint32 summationRiskScore READ summationRiskScore WRITE setSummationRiskScore NOTIFY summationRiskScoreChanged)
public:
    explicit ExposureSummary(QObject *parent = nullptr);
    ExposureSummary(ExposureSummary const &exposureSummary);
    ExposureSummary& operator=( const ExposureSummary &other);
    bool operator==( const ExposureSummary &other);
    bool operator!=( const ExposureSummary &other);

    quint32 daysSinceLastExposure() const;
    quint32 matchedKeyCount() const;
    qint32 maximumRiskScore() const;
    QList<qint32> attenuationDurations() const;
    qint32 summationRiskScore() const;

    void setDaysSinceLastExposure(quint32 daysSinceLastExposure);
    void setMatchedKeyCount(quint32 matchedKeyCount);
    void setMaximumRiskScore(qint32 maximumRiskScore);
    void setAttenuationDurations(QList<qint32> attenuationDurations);
    void setSummationRiskScore(qint32 summationRiskScore);

signals:
    void daysSinceLastExposureChanged();
    void matchedKeyCountChanged();
    void maximumRiskScoreChanged();
    void attenuationDurationsChanged();
    void summationRiskScoreChanged();

public slots:
private:
    quint32 m_daysSinceLastExposure;
    quint32 m_matchedKeyCount;
    qint32 m_maximumRiskScore;
    QList<qint32> m_attenuationDurations;
    qint32 m_summationRiskScore;
};

QDBusArgument &operator<<(QDBusArgument &argument, const ExposureSummary &exposureSummary);
QDBusArgument const &operator>>(const QDBusArgument &argument, ExposureSummary &exposureSummary);
QDataStream &operator<<(QDataStream &out, const ExposureSummary &exposureSummary);
QDataStream &operator>>(QDataStream &in, ExposureSummary &exposureSummary);

Q_DECLARE_METATYPE(ExposureSummary)

#endif // EXPOSURESUMMARY_H
