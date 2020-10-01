#ifndef TEST_STORAGE_H
#define TEST_STORAGE_H

#include <QObject>

class Test_Tracing : public QObject
{
    Q_OBJECT
public:
    explicit Test_Tracing(QObject *parent = nullptr);

private slots:
    void init();
    void cleanup();

    void testDtk();
    void testRpi();
    void testMetadata();
    void testMatchAggregation();
    void testBloomFilter();
    void testStorage();
    void testMatch();
    void testDiagnosis();
    void testDownloadConfig();
};

#endif // TEST_STORAGE_H
