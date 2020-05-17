#ifndef EXPOSURESUMMARY_H
#define EXPOSURESUMMARY_H

#include <QObject>

class ExposureSummary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 daysSinceLastExposure READ daysSinceLastExposure WRITE setDaysSinceLastExposure NOTIFY daysSinceLastExposureChanged)
    Q_PROPERTY(quint32 matchedKeyCount READ matchedKeyCount WRITE setMatchedKeyCount NOTIFY matchedKeyCountChanged)
    Q_PROPERTY(quint32 maximumRiskScore READ maximumRiskScore WRITE setMaximumRiskScore NOTIFY maximumRiskScoreChanged)
    Q_PROPERTY(QList<quint32> attenuationDurations READ attenuationDurations WRITE setAttenuationDurations NOTIFY attenuationDurationsChanged)
    Q_PROPERTY(quint32 summationRiskScore READ summationRiskScore WRITE setSummationRiskScore NOTIFY summationRiskScoreChanged)
public:
    explicit ExposureSummary(QObject *parent = nullptr);
    explicit ExposureSummary(ExposureSummary const &exposureSummary);

    quint32 daysSinceLastExposure() const;
    quint32 matchedKeyCount() const;
    quint32 maximumRiskScore() const;
    QList<quint32> attenuationDurations() const;
    quint32 summationRiskScore() const;

    void setDaysSinceLastExposure(quint32 daysSinceLastExposure);
    void setMatchedKeyCount(quint32 matchedKeyCount);
    void setMaximumRiskScore(quint32 maximumRiskScore);
    void setAttenuationDurations(QList<quint32> attenuationDurations);
    void setSummationRiskScore(quint32 summationRiskScore);

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
    quint32 m_maximumRiskScore;
    QList<quint32> m_attenuationDurations;
    quint32 m_summationRiskScore;
};

#endif // EXPOSURESUMMARY_H
