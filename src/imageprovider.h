#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>

class Settings;

class ImageProvider : public QQuickImageProvider {
public:
    explicit ImageProvider(Settings const &settings);

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
private:
    QString const m_imageDir;
};

#endif // IMAGEPROVIDER_H
