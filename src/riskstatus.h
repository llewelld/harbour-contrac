#ifndef RISKSTATUS_H
#define RISKSTATUS_H

#include <QObject>

class ExposureSummary;

class RiskStatus : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double combinedRiskScore READ combinedRiskScore NOTIFY combinedRiskScoreChanged)
    Q_PROPERTY(qint32 riskClassIndex READ riskClassIndex NOTIFY riskClassChanged)
    Q_PROPERTY(QString riskClassLabel READ riskClassLabel NOTIFY riskClassChanged)

public:
    explicit RiskStatus(QObject *parent = nullptr);

    qint32 totalRiskScore() const;
    double combinedRiskScore() const;
    qint32 riskClassIndex() const;
    QString riskClassLabel() const;

signals:
    void totalRiskScoreChanged();
    void combinedRiskScoreChanged();
    void riskClassChanged();

private slots:
    void recalculate();
    void updateExposureSummary();

private:
    double calculateRisk(qint32 combinedRiskScore, QList<qint32> const &attenuationDurations) const;
    qint32 calculateRiskClassIndex(double combinedRiskScore) const;
    QString calculateRiskClassLabel(double combinedRiskScore) const;

private:
    double m_combinedRiskScore;
    qint32 m_maximumRiskScore;
    QList<qint32> m_attenuationDurations;
};

#endif // RISKSTATUS_H
