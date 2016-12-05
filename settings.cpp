#include "settings.h"

Settings::Settings()
{
    sortDirectoriesFirst = true;
    exploreAppDirs = false;
    fileDisplayFilters = QDir::Hidden | QDir::System;
    sortFlags = QDir::Type | QDir::DirsFirst | QDir::IgnoreCase;
    colorContent1 = QColor(255, 255, 255);
    colorContent2 = QColor(240, 240, 240);
}

