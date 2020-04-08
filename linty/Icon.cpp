#include "Icon.h"
#include <QString>
#include <QImage>
#include <QDebug>
#include "Log.h"

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
        DEBUG_LOG("### Failed to load image: " + path);
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
    m_icons[ICON_UNKNOWN] = loadIcon(":/images/info.png");
    m_icons[ICON_APPLICATION] = loadIcon(":/images/lint.png");
    m_icons[ICON_REFRESH] = loadIcon(":/images/refresh.png");
}
