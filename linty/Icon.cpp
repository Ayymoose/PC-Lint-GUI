#include "Icon.h"
#include <QString>
#include <QImage>
#include <QDebug>

Icon::~Icon()
{
    qDeleteAll(m_icons);
    m_icons.clear();
}

QImage* Icon::loadIcon(const QString& path)
{
    QImage *img = new QImage();
    if (!img->load(path))
    {
        qDebug() << "Failed to load image: " << path;
        delete img;
        return nullptr;
    }
    else
    {
        return img;
    }
}

QImage* Icon::operator[](ICONS icon)
{
    Q_ASSERT(icon < ICON_COUNT);
    return m_icons[icon];
}

void Icon::loadIcons()
{
    m_icons[ICON_WARNING] = loadIcon(":/images/warning.png");
    m_icons[ICON_ERROR] = loadIcon(":/images/error.png");
    m_icons[ICON_INFORMATION] = loadIcon(":/images/info.png");
}
