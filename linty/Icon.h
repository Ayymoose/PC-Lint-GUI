#ifndef ICON_H
#define ICON_H

#include <QString>
#include <QImage>
#include <QMap>

enum ICONS
{
    ICON_WARNING = 0,
    ICON_ERROR,
    ICON_INFORMATION,
    ICON_UNKNOWN,
    ICON_APPLICATION,
    ICON_REFRESH,
    ICON_COUNT
};

class Icon
{
public:
    void loadIcons();
    Icon() = default;
    ~Icon();
    QImage* operator[](ICONS icon);
private:
    QImage* loadIcon(const QString& path);
    QMap<ICONS, QImage*> m_icons;
};


// You load a bunch of icons and put them into a map
// then access by Icons[Icon name]

#endif // ICON_H
