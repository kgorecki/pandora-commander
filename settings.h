#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>
#include <QVector>

#include "common.h"

struct PanelSettings
{
    int sortType;
    QString path;
};

class Settings
{
public:
    Settings();
    void ReadConfig();
    void WriteConfig();

    bool sortDirectoriesFirst;
    bool exploreAppDirs;
    QDir::Filters fileDisplayFilters;
    QDir::SortFlags sortFlags;

    QColor colorContent1, colorContent2;

    int tabsCount[kPanelsCount];
    QVector <PanelSettings> tabs[kPanelsCount];
};

#endif // SETTINGS_H
