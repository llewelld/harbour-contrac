#ifndef TEST_STORAGE_H
#define TEST_STORAGE_H

#include <QObject>

class Test_Storage : public QObject
{
    Q_OBJECT
public:
    explicit Test_Storage(QObject *parent = nullptr);

private slots:
    void cleanup();

    void testDtk();
    void testRpi();
    void testBloomFilter();
    void testStorage();
    void testMatch();
};

#endif // TEST_STORAGE_H
