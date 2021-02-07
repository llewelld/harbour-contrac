#ifndef RISCSCORECLASS_H
#define RISCSCORECLASS_H

#include <QObject>

namespace diagnosis {
class RiskScoreClass;
}

class RiskScoreClass : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(qint32 min READ min WRITE setMin NOTIFY minChanged)
    Q_PROPERTY(qint32 max READ max WRITE setMax NOTIFY maxChanged)

public:
    explicit RiskScoreClass(QObject *parent = nullptr);
    RiskScoreClass(RiskScoreClass const &riskScoreClass);
    RiskScoreClass(::diagnosis::RiskScoreClass const &riskScoreClass, QObject *parent = nullptr);

    RiskScoreClass &operator=(const RiskScoreClass &other);
    bool operator==(const RiskScoreClass &other) const;
    bool operator!=(const RiskScoreClass &other) const;

    bool operator==(const ::diagnosis::RiskScoreClass &other) const;
    bool operator!=(const ::diagnosis::RiskScoreClass &other) const;

    Q_INVOKABLE QString label() const;
    Q_INVOKABLE qint32 min() const;
    Q_INVOKABLE qint32 max() const;

public slots:
    void setLabel(QString label);
    void setMin(qint32 min);
    void setMax(qint32 max);

signals:
    void labelChanged();
    void minChanged();
    void maxChanged();

private:
    QString m_label;
    qint32 m_min;
    qint32 m_max;

signals:

public slots:
};

QDataStream &operator<<(QDataStream &out, const RiskScoreClass &riskScoreClass);
QDataStream &operator>>(QDataStream &in, RiskScoreClass &riskScoreClass);

Q_DECLARE_METATYPE(RiskScoreClass)

#endif // RISCSCORECLASS_H
