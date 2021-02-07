#include <sailfishapp.h>
#include <QColor>
#include <QDebug>
#include <QPainter>
#include <QQuickImageProvider>

#include "appsettings.h"

#include "imageprovider.h"

ImageProvider::ImageProvider(AppSettings const &settings)
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
    , m_imageDir(settings.getImageDir())
{
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap image;
    QStringList parts = id.split('?');
    QPixmap sourcePixmap(m_imageDir + parts.at(0) + ".png");

    if (size) {
        *size = sourcePixmap.size();
    }
    if (parts.length() > 1) {
        if (QColor::isValidColor(parts.at(1))) {
            QPainter painter(&sourcePixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(sourcePixmap.rect(), parts.at(1));
            painter.end();
        }
    }
    if (requestedSize.width() > 0 && requestedSize.height() > 0) {
        image = sourcePixmap.scaled(requestedSize.width(), requestedSize.height(), Qt::IgnoreAspectRatio);
    }
    else {
        image = sourcePixmap;
    }

    return image;
}
