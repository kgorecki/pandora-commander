#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QShortcut>

#include "common.h"
#include "settings.h"

enum PrevOperations
{
  POIdle = 0,
  PODirUp,
  PODirCurrent
};

struct PanelContainer
{
    PanelContainer(QString path);
    QWidget *mainView;
    QVBoxLayout *vblLayout;
    QLineEdit *lePath;
    QTableWidget *twFiles;
    QLabel *lblSummary;
    //QString currentPath;
    QString label;
    QDir *currentDir;
    QDir::SortFlags sortFlags;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class FileOperations;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openFile(int row, int column);

    //menu
    void sFileClose();
    void sEditAddTab();
    void sEditRemoveTab();
    void sCommandsCopyFiles();
    void sCommandsMkDir();
    void sCommandsMoveFiles();
    void sViewMaximize();
    void sViewMinimize();
    void sViewFullScreen();
    void sHelpAbout();

    //shortcuts
    void sTab();
    void sCtrlTab();
    void sCtrlShiftTab();
    void sHome();
    void sEnd();
    //file operations
//    void sCopyFiles();
//    void sMkDir();
//    void sMoveFiles();

public:
    void addTabs();
    static QLayout *createLayout(LayoutType layoutType);
    void doCopyMove(int operation);
    void doMkDir();
    QString getFileAttributes(QFileInfo file);
    void InsertRow(QTableWidget *widget, QTableWidgetItem *newItem, QTableWidgetItem *extItem, QTableWidgetItem *sizeItem, QTableWidgetItem *dateItem, int &parity);
    void FillDir(QTableWidget *widget, QFileInfoList &dirs);
    void FillFile(QTableWidget *widget, QFileInfoList &files);
    void FillTableWidget(QTableWidget *widget, QString path, QDir::SortFlags sortFlags);
    void NewTab(QString path, int panel);
    void OpenFile(int row, int panel);

    void PrepareActions();
    void PrepareActionsCommands();
    void PrepareActionsView();
    void PrepareButtons();
    void PrepareGlobalSettings();
    void PrepareMenus();
    void PrepareShortcuts();
    void PrepareTableWidget(QTableWidget *widget);

private:
    int     parity;
    QString pcVersion;
    QString prevDir;
    int     prevOperation;

    //main window
    QWidget *mainWidget;
    QVBoxLayout *ltMainWindow;
    QWidget *wPanels;
    QHBoxLayout *ltPanels;
    QList <PanelContainer *> tabs[kPanelsCount];
    QTabWidget *tab[kPanelsCount];
    QLabel *lblPropmt;
    QComboBox *cbPrompt;
    QHBoxLayout *ltPrompt;
    QWidget *wPrompt;
    QPushButton *pbCopy;
    QPushButton *pbMove;
    QPushButton *pbMkDir;
    QPushButton *pbDelete;
    QHBoxLayout *ltButtons;
    QWidget *wButtons;

    //menu
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuCommands;
    QMenu *menuView;
    QMenu *menuHelp;
    QAction *aFileClose;
    QAction *aEditAddTab;
    QAction *aEditRemoveTab;
    QAction *aViewMaximize;
    QAction *aViewMinimize;
    QAction *aViewFullScreen;
    QAction *aHelpAbout;
    QAction *aCommandsCopyFiles;
    QAction *aCommandsMoveFiles;
    QAction *aCommandsMkDir;
    QAction *aOpenFile;

    //shortcuts
    QShortcut *scTab;
    QShortcut *scCtrlTab;
    QShortcut *scCtrlShiftTab;
    QShortcut *scHome;
    QShortcut *scEnd;
    QShortcut *scEnter;
    //file operations
//    QShortcut *scCopyFiles;
//    QShortcut *scMoveFiles;
//    QShortcut *scMkDir;

    Settings globalSettings;

    int tabsCount[kPanelsCount];
    int currentPanel;
};

#endif // MAINWINDOW_H
