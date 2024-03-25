#include "mainwindow.h"

#include <QDateTime>
#include <QHeaderView>
#include <QDesktopServices>
#include <QMenuBar>
#include <QMessageBox>
#include <QUrl>
#include <QShortcut>

#include "fileoperations.h"

PanelContainer::PanelContainer(QString path)
{
    mainView = new QWidget();
    vblLayout = (QVBoxLayout *) MainWindow::createLayout(LayoutVBox);
    lePath = new QLineEdit();
    twFiles = new QTableWidget();
    lblSummary = new QLabel();
    //currentPath = path;
    currentDir = new QDir(path);
    label = getLabel(currentDir);
    sortFlags = QDir::Type | QDir::DirsFirst | QDir::IgnoreCase;

    twFiles->setSelectionBehavior(QAbstractItemView::SelectRows);

    vblLayout->addWidget(lePath);
    vblLayout->addWidget(twFiles);
    vblLayout->addWidget(lblSummary);

    mainView->setLayout(vblLayout);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    pcVersion = tr("0.0.1 alpha");
    prevOperation = POIdle;
    PrepareGlobalSettings();
    tabsCount[kPanelLeft] = 0;
    tabsCount[kPanelRight] = 0;
    tab[kPanelLeft] = new QTabWidget();
    tab[kPanelRight] = new QTabWidget();
    mainWidget = new QWidget();
    this->resize(800, 600);
    this->setWindowTitle(tr("Pandora Commander"));
    this->setCentralWidget(mainWidget);
    ltMainWindow = (QVBoxLayout *) createLayout(LayoutVBox);
    addTabs();

    wPanels = new QWidget();
    ltPanels = (QHBoxLayout *) createLayout(LayoutHBox);
    wPanels->setLayout(ltPanels);

    lblPropmt = new QLabel(tabs[0][0]->currentDir->absolutePath());
    lblPropmt->setAlignment(Qt::AlignRight);
    cbPrompt = new QComboBox();
    ltPrompt = (QHBoxLayout *)createLayout(LayoutHBox);
    wPrompt = new QWidget();

    ltPrompt->addWidget(lblPropmt);
    ltPrompt->addWidget(cbPrompt);
    wPrompt->setLayout(ltPrompt);

    PrepareButtons();

    ltPanels->addWidget(tab[kPanelLeft]);
    ltPanels->addWidget(tab[kPanelRight]);
    ltMainWindow->addWidget(wPanels);
    ltMainWindow->addWidget(wPrompt);
    ltMainWindow->addWidget(wButtons);
    mainWidget->setLayout(ltMainWindow);

    PrepareActions();
    PrepareMenus();
    PrepareShortcuts();
    tabs[kPanelLeft][0]->twFiles->setFocus();
    currentPanel = kPanelLeft;
}

void MainWindow::addTabs()
{
#ifdef Q_OS_WIN32
    NewTab(tr("c:\\"), kPanelLeft);
    NewTab(tr("c:\\"), kPanelRight);
#else
    NewTab(tr("/"), kPanelLeft);
    NewTab(tr("/"), kPanelRight);
#endif
}

QLayout *MainWindow::createLayout(LayoutType layoutType)
{
    QLayout *toCreate;
    if (LayoutVBox == layoutType)
        toCreate = new QVBoxLayout();
    else //if (LayoutHBox == layoutType)
        toCreate = new QHBoxLayout();
    toCreate->setSpacing(1);
    toCreate->setContentsMargins(1,1,1,1);
    return toCreate;
}

void MainWindow::openFile(int row, int /*column*/)
{
    for (int i = 0; i < kPanelsCount; i++)
    {
        int size = tabs[i].size();
        for (int j = 0; j < size; j++)
        {
            PanelContainer *container = tabs[i][j];
            if (container->twFiles->hasFocus())
            {
                QTableWidgetItem *item = container->twFiles->item(row, kColumnName);
                QTableWidgetItem *itemExt = container->twFiles->item(row, kColumnExtension);
                QString ext = "";
                if ("" != itemExt->text())
                    ext += "." + itemExt->text();

                if (container->twFiles->item(row, kColumnSize)->text().compare("<DIR>"))
                {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(container->currentDir->absoluteFilePath(item->text() + ext)));
                }
                else
                {
#ifdef __APPLE__
                    if (item->text().endsWith(tr(".app")))
                    {
                        QDesktopServices::openUrl(QUrl::fromLocalFile(container->currentDir->absoluteFilePath(item->text())));
                        break;
                    }
#endif
                    if (!container->currentDir->cd(item->text()))
                        return;
                    if (".." == item->text())
                    {
                        prevOperation = PODirUp;
                        prevDir = container->label;
                        //QMessageBox::about(this, tr(".."), tr(".."));
                    }
                    container->label = getLabel(container->currentDir);
                    tab[i]->setTabText(j, container->label);
                    container->lePath->setText(container->currentDir->absolutePath());
                    FillTableWidget(container->twFiles, container->currentDir->absolutePath(), container->sortFlags);
                }
                break;
            }
        }
    }
}

void MainWindow::sFileClose()
{
    this->close();
}

void MainWindow::sEditAddTab()
{
    for (int i = 0; i < kPanelsCount; i++)
    {
        if (tabs[i][tab[i]->currentIndex()]->twFiles->hasFocus())
        {
            NewTab(tabs[i][tab[i]->currentIndex()]->currentDir->absolutePath(), i);
            break;
        }
    }
}

void MainWindow::sEditRemoveTab()
{
    for (int i = 0; i < kPanelsCount; i++)
    {
        if (tabs[i][tab[i]->currentIndex()]->twFiles->hasFocus())
        {
            if (tabsCount[i] > 1)
            {
                tab[i]->removeTab(tab[i]->currentIndex());
                tabs[i].removeAt(tab[i]->currentIndex());
                --tabsCount[i];
            }
            tabs[i][tab[i]->currentIndex()]->twFiles->setFocus();
            break;
        }
    }
}

void MainWindow::sCommandsCopyFiles()
{
    doCopyMove(kCopy);
}

void MainWindow::sCommandsMkDir()
{
    doMkDir();
}

void MainWindow::sCommandsMoveFiles()
{
    doCopyMove(kRenMove);
}

void MainWindow::sViewMaximize()
{
    if (this->isMaximized())
        this->showNormal();
    else
        this->showMaximized();
}

void MainWindow::sViewMinimize()
{
    this->showMinimized();
}

void MainWindow::sViewFullScreen()
{
    if (this->isFullScreen())
        this->showNormal();
    else
        this->showFullScreen();
}

void MainWindow::sHelpAbout()
{
    QMessageBox::about(this, tr("About Pandora Commander"),
                tr("The <b>Pandora Commander</b> version ") + pcVersion
                       + tr("<br>Author: Krzysztof Gorecki kgorecki@b-intohimo.com"));
}

void MainWindow::sTab()
{
    currentPanel = (currentPanel + 1) % kPanelsCount;
    tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->setFocus();
}

void MainWindow::sCtrlTab()
{
    tab[currentPanel]->setCurrentIndex((tab[currentPanel]->currentIndex() + 1) % tabsCount[currentPanel]);
    tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->setFocus();
}

void MainWindow::sCtrlShiftTab()
{
    tab[currentPanel]->setCurrentIndex((tab[currentPanel]->currentIndex() + tabsCount[currentPanel] - 1) % tabsCount[currentPanel]);
    tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->setFocus();
}

void MainWindow::sHome()
{
    tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->selectRow(0);
}

void MainWindow::sEnd()
{
    tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->selectRow(tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->rowCount() - 1);
}

void MainWindow::doCopyMove(int operation)
{
    QString source = tabs[currentPanel][tab[currentPanel]->currentIndex()]->lePath->text();
    QString destination = tabs[(currentPanel + 1) % kPanelsCount][tab[(currentPanel + 1) % kPanelsCount]->currentIndex()]->lePath->text();

    QList<QTableWidgetItem *> items = tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->selectedItems();
    QList<QString> *files = new QList<QString>;
    QList<QTableWidgetItem *>::iterator iterator = items.begin();
    int size = items.count() / kActiveColumnCount;
    for (int i = 0; i < size; i++)
    {
        QString newItem = (*iterator)->text();
        int rowNumber = tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->row(*iterator);
        if ((tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->item(rowNumber, kColumnExtension))->text().compare(""))
            newItem.append("." + (tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->item(rowNumber, kColumnExtension))->text());
        (*files).push_back(newItem);
        iterator++;
    }

    FileOperations *copyMove = new FileOperations(this);
    copyMove->CopyMoveFiles(files, source, destination, (currentPanel + 1) % kPanelsCount, tab[(currentPanel + 1) % kPanelsCount]->currentIndex(), operation);
}

void MainWindow::doMkDir()
{
    QString curDir = tabs[currentPanel][tab[currentPanel]->currentIndex()]->lePath->text();

    FileOperations *mkDir = new FileOperations(this);
    mkDir->MkDir(curDir);
}

QString MainWindow::getFileAttributes(QFileInfo file)
{
    QString result;

    if (file.isHidden())
        result += "h";
    else
        result += "-";
    if (file.isWritable())
        result += "-";
    else
        result += "r";

    return result;
}

void MainWindow::InsertRow(QTableWidget *widget, QTableWidgetItem *newItem, QTableWidgetItem *extItem, QTableWidgetItem *sizeItem, QTableWidgetItem *dateItem, int &parity)
{
    newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
    extItem->setFlags(extItem->flags() ^ Qt::ItemIsEditable);
    sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);
    dateItem->setFlags(dateItem->flags() ^ Qt::ItemIsEditable);

    if (parity)
    {
        newItem->setBackground(globalSettings.colorContent1);
        extItem->setBackground(globalSettings.colorContent1);
        sizeItem->setBackground(globalSettings.colorContent1);
        dateItem->setBackground(globalSettings.colorContent1);
        parity = 0;
    }
    else
    {
        newItem->setBackground(globalSettings.colorContent2);
        extItem->setBackground(globalSettings.colorContent2);
        sizeItem->setBackground(globalSettings.colorContent2);
        dateItem->setBackground(globalSettings.colorContent2);
        parity = 1;
    }

    int row = widget->rowCount();
    widget->insertRow(row);
    widget->setRowHeight(row, widget->rowHeight(row) / 2);
    widget->setItem(row, kColumnName, newItem);
    widget->setItem(row, kColumnExtension, extItem);
    widget->setItem(row, kColumnSize, sizeItem);
    widget->setItem(row, kColumnModified, dateItem);
}

void MainWindow::FillDir(QTableWidget *widget, QFileInfoList &dirs)
{
    QFileInfoList::iterator currentFile = dirs.begin();
    if (!dirs.at(0).fileName().compare("."))
        ++currentFile;
    if ((dirs.at(1).path() == QDir::rootPath()))
        if (!dirs.at(1).fileName().compare(".."))
            ++currentFile;

    parity = 0;
    for (; currentFile != dirs.end(); currentFile++)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(currentFile->fileName());
        QTableWidgetItem *extItem = new QTableWidgetItem("");
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("<DIR>"));
        //currentFile = tr("[%1]").arg(currentFile.fileName());
        QDateTime fileDate = currentFile->lastModified();
        QTableWidgetItem *dateItem = new QTableWidgetItem(fileDate.toString("yyyy-MM-dd hh:mm:ss"));

        InsertRow(widget, newItem, extItem, sizeItem, dateItem, parity);
    }
}

void MainWindow::FillFile(QTableWidget *widget, QFileInfoList &files)
{
    for (QFileInfoList::iterator currentFile = files.begin(); currentFile != files.end(); currentFile++)
    {
        QTableWidgetItem *newItem;
        QTableWidgetItem *extItem;
        newItem = new QTableWidgetItem(currentFile->completeBaseName());
        if (currentFile->completeBaseName().isEmpty())
        {
            newItem = new QTableWidgetItem(currentFile->fileName());
            extItem = new QTableWidgetItem(tr(""));
        }
        else
        {
            extItem = new QTableWidgetItem(currentFile->suffix());
        }
        //currentFile = tr("[%1]").arg(currentFile.fileName());
        QDateTime fileDate = currentFile->lastModified();
        QTableWidgetItem *dateItem = new QTableWidgetItem(fileDate.toString("yyyy-MM-dd hh:mm:ss"));

        //TODO: filling file size:
        //configurable with or without rounding
        qint64 fileSize = currentFile->size();
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1").arg(fileSize));

        InsertRow(widget, newItem, extItem, sizeItem, dateItem, parity);
    }
}

void MainWindow::FillTableWidget(QTableWidget *widget, QString path, QDir::SortFlags /*sortFlags*/)
{
    widget->clearContents();
    widget->setRowCount(0);
    QDir dirLeft(path);

    QFileInfoList dirs = dirLeft.entryInfoList(globalSettings.fileDisplayFilters | QDir::AllDirs, globalSettings.sortFlags);

    FillDir(widget, dirs);

    QFileInfoList files = dirLeft.entryInfoList(globalSettings.fileDisplayFilters | QDir::Files, globalSettings.sortFlags);

    FillFile(widget, files);

    if (PODirUp == prevOperation)
    {
        prevOperation = POIdle;
        int rowCount = tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->rowCount();
        for (int i = 0; i < rowCount; i++)
            if (prevDir == tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->item(i, 0)->text())
                tabs[currentPanel][tab[currentPanel]->currentIndex()]->twFiles->setCurrentCell(i, 0);
    }
    else
        widget->selectRow(0);
}

void MainWindow::NewTab(QString path, int panel)
{
    PanelContainer *newPanel = new PanelContainer(path);
    PrepareTableWidget(newPanel->twFiles);
    newPanel->lePath->setText(newPanel->currentDir->absolutePath());
    tabs[panel].push_back(newPanel);
    FillTableWidget(newPanel->twFiles, path, newPanel->sortFlags);
    tab[panel]->addTab(newPanel->mainView, newPanel->label);
    ++tabsCount[panel];
}

void MainWindow::PrepareActions()
{
    aFileClose = new QAction(tr("Close"), this);
    aFileClose->setShortcut(QKeySequence::Quit);
    //aFileClose->setShortcut(tr("alt+f4"));
    aFileClose->setStatusTip(tr("Close program"));
    connect(aFileClose, SIGNAL(triggered()), this, SLOT(sFileClose()));

    aEditAddTab = new QAction(tr("Add tab"), this);
    aEditAddTab->setShortcut(tr("ctrl+t"));
    aEditAddTab->setStatusTip(tr("Add new tab to current panel"));
    connect(aEditAddTab, SIGNAL(triggered()), this, SLOT(sEditAddTab()));

    aEditRemoveTab = new QAction(tr("Remove tab"), this);
    aEditRemoveTab->setShortcut(tr("ctrl+w"));
    aEditRemoveTab->setStatusTip(tr("Removes tab from current panel"));
    connect(aEditRemoveTab, SIGNAL(triggered()), this, SLOT(sEditRemoveTab()));

    PrepareActionsCommands();
    PrepareActionsView();

    aHelpAbout = new QAction(tr("About"), this);
    aHelpAbout->setShortcut(tr("ctrl+/"));
    aHelpAbout->setStatusTip(tr("Opens About menu"));
    connect(aHelpAbout, SIGNAL(triggered()), this ,SLOT(sHelpAbout()));
}

void MainWindow::PrepareButtons()
{
    pbCopy = new QPushButton(tr("Copy"));
    pbMove = new QPushButton(tr("Move"));
    pbMkDir = new QPushButton(tr("MkDir"));
    pbDelete = new QPushButton(tr("Delete"));
    ltButtons = (QHBoxLayout *) createLayout(LayoutHBox);
    wButtons = new QWidget();
    ltButtons->addWidget(pbCopy);
    ltButtons->addWidget(pbMove);
    ltButtons->addWidget(pbMkDir);
    ltButtons->addWidget(pbDelete);
    wButtons->setLayout(ltButtons);

    connect(pbCopy, SIGNAL(released()), this, SLOT(sCommandsCopyFiles()));
    connect(pbMove, SIGNAL(released()), this, SLOT(sCommandsMoveFiles()));
    connect(pbMkDir, SIGNAL(released()), this, SLOT(sCommandsMkDir()));
    pbDelete->setDisabled(true);
}

void MainWindow::PrepareActionsCommands()
{
    aCommandsCopyFiles = new QAction(tr("Copy"), this);
    aCommandsCopyFiles->setShortcut(tr("f5"));
    aCommandsCopyFiles->setStatusTip(tr("Copy"));
    connect(aCommandsCopyFiles, SIGNAL(triggered()), this, SLOT(sCommandsCopyFiles()));

    aCommandsMoveFiles = new QAction(tr("Rename / move"), this);
    aCommandsMoveFiles->setShortcut(tr("f6"));
    aCommandsMoveFiles->setStatusTip(tr("Rename / move"));
    connect(aCommandsMoveFiles, SIGNAL(triggered()), this, SLOT(sCommandsMoveFiles()));

    aCommandsMkDir = new QAction(tr("Create directory"), this);
    aCommandsMkDir->setShortcut(tr("f7"));
    aCommandsMkDir->setStatusTip(tr("Create directory"));
    connect(aCommandsMkDir, SIGNAL(triggered()), this, SLOT(sCommandsMkDir()));
}

void MainWindow::PrepareActionsView()
{
    aViewMaximize = new QAction(tr("Maximize window"), this);
    aViewMaximize->setShortcut(tr("alt+f10"));
    aViewMaximize->setStatusTip(tr("Maximize window"));
    connect(aViewMaximize, SIGNAL(triggered()), this, SLOT(sViewMaximize()));

    aViewMinimize = new QAction(tr("Minimize window"), this);
    aViewMinimize->setShortcut(tr("alt+f9"));
    aViewMinimize->setStatusTip(tr("Minimize window"));
    connect(aViewMinimize, SIGNAL(triggered()), this, SLOT(sViewMinimize()));

    aViewFullScreen = new QAction(tr("FullScreen"), this);
    aViewFullScreen->setShortcut(tr("alt+f11"));
    aViewFullScreen->setStatusTip(tr("Switch full screen mode on/off"));
    connect(aViewFullScreen, SIGNAL(triggered()), this, SLOT(sViewFullScreen()));
}

void MainWindow::PrepareGlobalSettings()
{

}

void MainWindow::PrepareMenus()
{
    menuFile = menuBar()->addMenu((tr("&File")));
    menuFile->addAction(aFileClose);

    menuEdit = menuBar()->addMenu((tr("&Edit")));
    menuEdit->addAction(aEditAddTab);
    menuEdit->addAction(aEditRemoveTab);

    menuCommands = menuBar()->addMenu((tr("&Commands")));
    menuCommands->addAction(aCommandsCopyFiles);
    menuCommands->addAction(aCommandsMoveFiles);
    menuCommands->addAction(aCommandsMkDir);

    menuView = menuBar()->addMenu((tr("&View")));
    menuView->addAction(aViewMaximize);
    menuView->addAction(aViewMinimize);
    menuView->addAction(aViewFullScreen);

    menuHelp = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(aHelpAbout);
}

void MainWindow::PrepareShortcuts()
{
    scTab = new QShortcut(this);
    scTab->setKey(tr("tab"));
    scTab->setEnabled(true);
    connect(scTab, SIGNAL(activated()), this, SLOT(sTab()));

    scCtrlTab = new QShortcut(this);
    scCtrlTab->setKey(tr("ctrl+tab"));
    scCtrlTab->setEnabled(true);
    connect(scCtrlTab, SIGNAL(activated()), this, SLOT(sCtrlTab()));

    scCtrlShiftTab = new QShortcut(this);
    scCtrlShiftTab->setKey(tr("ctrl+shift+tab"));
    scCtrlShiftTab->setEnabled(true);
    connect(scCtrlShiftTab, SIGNAL(activated()), this, SLOT(sCtrlShiftTab()));

    scHome = new QShortcut(this);
    scHome->setKey(tr("home"));
    scHome->setEnabled(true);
    connect(scHome, SIGNAL(activated()), this, SLOT(sHome()));

    scEnd = new QShortcut(this);
    scEnd->setKey(tr("end"));
    scEnd->setEnabled(true);
    connect(scEnd, SIGNAL(activated()), this, SLOT(sEnd()));

    scEnter = new QShortcut(this);
    scEnter->setKey(tr("enter"));
    scEnter->setEnabled(true);
    connect(scEnter, SIGNAL(cellActivated(int,int)), this, SLOT(openFile(int,int)));
}

void MainWindow::PrepareTableWidget(QTableWidget *widget)
{
    widget->setColumnCount(kColumnCount);
    int width = widget->size().width();
    widget->setColumnWidth(kColumnName, (width * kColumnNameWidth) / 100);
    widget->setColumnWidth(kColumnExtension, (width * kColumnExtWidth) / 100);
    widget->setColumnWidth(kColumnSize, (width * kColumnSizeWidth) / 100);
    widget->setColumnWidth(kColumnModified, (width * kColumnModifiedWidth) / 100);
#ifdef Q_OS_WIN32
    widget->setColumnWidth(kColumnAttr, (width * kColumnAttrWidth) / 100);
#else
    widget->setColumnWidth(kColumnPermisions, (width * kColumnPermisionsWidth) / 100);
#endif

    QStringList labels;
    labels << tr("Name") << tr("Ext") << tr("Size") << tr("Modified")
#ifdef Q_OS_WIN32
            << tr("Attr");
#else
            << tr("Perm");
#endif

    widget->setHorizontalHeaderLabels(labels);
    widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    widget->verticalHeader()->hide();
    widget->setShowGrid(false);

    connect(widget, SIGNAL(cellActivated(int,int)), this, SLOT(openFile(int,int)));
//    connect(widget, SIGNAL(cellEntered(int,int)), this, SLOT(openFile(int,int)));
}

MainWindow::~MainWindow()
{
    for (int i = 0; i < kPanelsCount; i++)
    {
        for (int j = 0; j < tabsCount[i]; j++)
        {
            delete tabs[i][j];
        }
    }
}
