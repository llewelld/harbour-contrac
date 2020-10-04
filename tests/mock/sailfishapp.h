#ifndef MOCK_SAILFISHAPP_H
#define MOCK_SAILFISHAPP_H

#include <QObject>
#include <QUrl>

class SailfishApp : public QObject
{
    Q_OBJECT

public:
    static QUrl pathTo(QString const path)
    {
        return QUrl(QStringLiteral("./") + path);
    }
};

#endif // MOCK_SAILFISHAPP_H

