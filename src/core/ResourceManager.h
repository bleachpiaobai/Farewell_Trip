#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <QHash>
#include <QPixmap>
#include <QString>

class ResourceManager : public QObject
{
    Q_OBJECT
public:
    explicit ResourceManager(QObject* parent = nullptr);

    // Image resources
    bool loadImage(const QString& key, const QString& path);
    QPixmap image(const QString& key) const;
    bool hasImage(const QString& key) const;

    // Stylesheet
    void loadStyleSheet(const QString& path);

    // Convenience
    QString resourcePath(const QString& relative) const;

private:
    QHash<QString, QPixmap> m_images;
};

#endif // RESOURCEMANAGER_H
