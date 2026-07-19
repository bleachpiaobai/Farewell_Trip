#include "ResourceManager.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

ResourceManager::ResourceManager(QObject* parent)
    : QObject(parent)
{
}

bool ResourceManager::loadImage(const QString& key, const QString& path)
{
    QPixmap pix(path);
    if (pix.isNull()) {
        qWarning() << "ResourceManager: failed to load" << path;
        return false;
    }
    m_images[key] = pix;
    return true;
}

QPixmap ResourceManager::image(const QString& key) const
{
    return m_images.value(key);
}

bool ResourceManager::hasImage(const QString& key) const
{
    return m_images.contains(key);
}

void ResourceManager::loadStyleSheet(const QString& path)
{
    QFile f(path);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString style = QString::fromUtf8(f.readAll());
        qApp->setStyleSheet(style);
    }
}

QString ResourceManager::resourcePath(const QString& relative) const
{
    return QStringLiteral(":/%1").arg(relative);
}
