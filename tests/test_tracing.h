#ifndef TEST_STORAGE_H
#define TEST_STORAGE_H

#include <QObject>

class Test_Tracing : public QObject
{
    Q_OBJECT
public:
    explicit Test_Tracing(QObject *parent = nullptr);

private slots:
    void cleanup();

    void testDtk();
    void testRpi();
    void testBloomFilter();
    void testStorage();
    void testMatch();
};

#endif // TEST_STORAGE_H
