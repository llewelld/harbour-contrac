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

    void testStorage();
};

#endif // TEST_STORAGE_H
