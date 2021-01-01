#include <QDebug>

#include "appsettings.h"
#include "sfsecrethelper.h"
#include "testresult.h"
#include "upload.h"

TestResult::TestResult(QObject *parent)
    : QObject(parent)
    , m_result(Result::NoResult)
{
    if (this->possiblyAvailable()) {
        SFSecretHelper::Response res = SFSecretHelper::getInstance().get(TESTRESULT_KEY);
        if (res.returncode == 0) {
            m_result = static_cast<Result>(res.data.toInt());
        }
        else {
            qWarning() << "Couldn't retrieve testresult token from Sailfish Secrets Storage!";
            //Change it to changed so that the initial value NoResult gets stored on destruction
            m_result_changed = true;
        }
    }
}

TestResult::~TestResult()
{
    if (m_result_changed) {
        if (SFSecretHelper::getInstance().remove(TESTRESULT_KEY).returncode != 0) {
            qWarning() << "Couldn't delete old Testresult out of Sailfish Secrets Storage!";
        }
        if (SFSecretHelper::getInstance().store(TESTRESULT_KEY, QByteArray::number(static_cast<int>(m_result))).returncode != 0) {
            qWarning() << "Couldn't store current Testresult in Sailfish Secrets Storage!";
        }
    }
}

void TestResult::startCheck()
{
    if (TestResult::possiblyAvailable()) {
        if (AppSettings::getInstance().regTokenReceived().daysTo(QDate::currentDate()) > 21) {
            m_result = Result::NoResult;
            emit testResultChanged();
            int returncode;
            returncode = SFSecretHelper::getInstance().remove(REGTOKEN_KEY).returncode;
            returncode += SFSecretHelper::getInstance().remove(TESTRESULT_KEY).returncode;
            if (returncode == 0) {
                //Default QDate() creates an invalid date
                AppSettings::getInstance().setRegTokenReceived(QDate());
                emit possiblyAvailableChanged();
                if (SFSecretHelper::getInstance().store(TESTRESULT_KEY, QByteArray::number(static_cast<int>(m_result))).returncode != 0) {
                    qWarning() << "Couldn't store current Testresult in Sailfish Secrets Storage!";
                    m_result_changed = true;
                }
            }
            else {
                qDebug() << "Couldn't delete expired registry token and testresult from Sailfish Secrets Storage!";
                m_result_changed = true;
            }
            return;
        }
        if (m_result == Result::NoResult || m_result == Result::Pending) {
            SFSecretHelper::Response res = SFSecretHelper::getInstance().get(REGTOKEN_KEY);
            if (res.returncode == 0) {
                emit testResultRequested(QString::fromLatin1(res.data));
            }
            else {
                qDebug() << "Couldn't retrieve registry token from Sailfish Secrets Storage!";
            }
        }
    }
}

QString TestResult::testClassLabel() const
{
    return m_resultLabels.at(static_cast<int>(m_result));
}

bool TestResult::possiblyAvailable() const
{
    return AppSettings::getInstance().regTokenReceived().isValid();
}

TestResult::Result TestResult::currentResult() const
{
    return m_result;
}

void TestResult::testResultDownloaded(const int &result)
{
    //Directly map values from Android App
    // https://github.com/corona-warn-app/cwa-app-android/blob/4f7664467cb9be280f39e5e61f9e0eb5f6063613/Corona-Warn-App/src/main/java/de/rki/coronawarnapp/util/formatter/TestResult.kt
    m_result = static_cast<Result>(result);
    m_result_changed = true;
    emit testResultChanged();
    qDebug() << "Test result downloaded: " << this->testClassLabel() << " Original number: " << result;
}

void TestResult::regTokenWasReceived()
{
    emit possiblyAvailableChanged();
}
