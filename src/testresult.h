#ifndef TESTRESULT_H
#define TESTRESULT_H

#include <QObject>

const QString TESTRESULT_KEY = QStringLiteral("testresult");
const QString REGTOKEN_KEY = QStringLiteral("regtoken");

class TestResult : public QObject
{
    Q_OBJECT
    Q_ENUMS(Result)
    Q_PROPERTY(QString testClassLabel READ testClassLabel NOTIFY testResultChanged)
    Q_PROPERTY(Result currentResult READ currentResult NOTIFY testResultChanged)
    Q_PROPERTY(bool possiblyAvailable READ possiblyAvailable NOTIFY possiblyAvailableChanged)

public:
    enum class Result
    {
        Pending,
        Negative,
        Positive,
        Invalid,
        Redeemed,
        NoResult
    };
    explicit TestResult(QObject *parent = nullptr);
    ~TestResult();

    QString testClassLabel() const;
    Result currentResult() const;
    bool possiblyAvailable() const;

    Q_INVOKABLE void startCheck();
    Q_INVOKABLE void testResultDownloaded(const int &result);
    Q_INVOKABLE void regTokenWasReceived();

signals:
    void possiblyAvailableChanged();
    void testResultRequested(QString regToken);
    void testResultChanged();

private:
    Result m_result;
    bool m_result_changed;
    QStringList m_resultLabels{"Pending", "Negative", "Positive", "Invalid", "Redeemed", "No Result"};
};

#endif // TESTRESULT_H
