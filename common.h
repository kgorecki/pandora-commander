#ifndef COMMON_H
#define COMMON_H

#include <QDir>
#include <QLayout>

enum ePanel
{
    kPanelLeft = 0,
    kPanelRight
};

enum eColumn
{
    kColumnName = 0,
    kColumnExtension,
    kColumnSize,
    kColumnModified,
#ifdef Q_OS_WIN32
    kColumnAttr
#else
    kColumnPermisions
#endif
};

enum LayoutType
{
    LayoutVBox = 0,
    LayoutHBox
};

const int kPanelsCount = 2;
const int kColumnCount = 5;
const int kActiveColumnCount = 4;
#ifdef Q_OS_WIN32
const int kRootDirCharsCount = 3;
#else
const int kRootDirCharsCount = 1;
#endif

const int kColumnNameWidth = 65;
const int kColumnExtWidth = 10;
const int kColumnSizeWidth = 8;
const int kColumnModifiedWidth = 12;
#ifdef Q_OS_WIN32
const int kColumnAttrWidth = 5;
#else
const int kColumnPermisionsWidth = 5;
#endif

inline QString getLabel(QDir *currentDir)
{
    if (currentDir->isRoot())
        return currentDir->absolutePath().left(kRootDirCharsCount);
    else
        return currentDir->dirName();
}

#endif // COMMON_H
