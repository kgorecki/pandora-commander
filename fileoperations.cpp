#include "fileoperations.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "mainwindow.h"

int FileOperations::operationCount = 0;

FileOperations::FileOperations(QWidget *parent) : QDialog(parent)
{
    mainLayout = NULL;
    //przysicki: drzewo lub kolumny do wyboru celu
    lblSource = NULL;
    lblDestination = NULL;
    leSource = NULL;
    leDestination = NULL;
    buttons = NULL;
    buttonsLayout = NULL;
    pbOk = NULL;
    pbCancel = NULL;

    filesToCopy = 0;

}

void FileOperations::CopyMoveFiles(QList<QString> *files, QString source, QString destination, int destPanel, int destTab, int operation)
{
    this->files = files;
    this->source = source;
    this->destination = destination;
    this->destPanel = destPanel;
    this->destTab = destTab;
    this->operation = operation;
    drawCopyMoveFiles();
}

void FileOperations::MkDir(QString curDir)
{
    this->source = curDir;
    drawMkDir();
}

void FileOperations::okProceed()
{
    //--operationCount;
    this->setWindowModality(Qt::WindowModal);
    clearAll();
    if ((kCopy == this->operation) || (kRenMove == this->operation))
    {
        drawCopyMoveProgress();
        doCopyMoveFiles();
    }
    else if (kMkDir == this->operation)
        doMkDir(this->source);
}

void FileOperations::cancel()
{
    --operationCount;
    this->close();
}

void FileOperations::clearAll()
{
    this->close();
    if (mainLayout)
        delete mainLayout;
    //przysicki: drzewo lub kolumny do wyboru celu
    if (lblSource)
        delete lblSource;
    if (lblDestination)
        delete lblDestination;
    if (leSource)
        delete leSource;
    if (leDestination)
        delete leDestination;
    if (buttons)
        delete buttons;
    //if (buttonsLayout)
    //    delete buttonsLayout;
    //if (pbOk)
    //    delete pbOk;
    //if (pbCancel)
    //    delete pbCancel;
}

void FileOperations::doCopyMoveDir(QString &path)
{
    QDir currentDir(pathCombine(destination, path));
    if (!currentDir.exists())
        if (!currentDir.mkpath(pathCombine(destination, path)))
            QMessageBox::warning(this, tr("Copy failed!"), tr("Copy failed! Error code: %1").arg(kCreateDirectoryFailed));

    QFileInfoList dirs = QDir(pathCombine(source, path)).entryInfoList(QDir::AllDirs);

    Q_FOREACH(QFileInfo dir, dirs)
    {
        QString fileName = dir.fileName();
        if (fileName.compare(".") && fileName.compare(".."))
        {
            QString pth = pathCombine(path, fileName);
            doCopyMoveDir(pth);
        }
    }

    QFileInfoList files = QDir(pathCombine(source, path)).entryInfoList(QDir::Files);
    Q_FOREACH(QFileInfo file, files)
    {
        QString fileName = file.fileName();
        QString tempPath = pathCombine(path, fileName);
        QString src = pathCombine(source, tempPath);
        QString dst = pathCombine(destination, tempPath);
        doCopyOneFile(src, dst);
    }
}

void FileOperations::doCopyMoveFiles()
{
    doFilesCount();

    int i = 1;
    for (QList<QString>::Iterator iterator = files->begin(); iterator != files->end(); iterator++)
    {
        QString src = pathCombine(source, *iterator);
        QString dst = pathCombine(destination, *iterator);
        QFile currentFile(src);
        lblSource->setText(src);
        lblDestination->setText(dst);
        if (kRenMove == operation)
        {
            QDir curDir(src);
            curDir.setPath(dst);
        }
        else
        {
            if (QFileInfo(currentFile).isDir())
                doCopyMoveDir(*iterator);
            else
            {
                if (int kErr = doCopyOneFile(src, dst))
                    QMessageBox::warning(this, tr("Copy failed!"), tr("Copy failed! Error code: %1").arg(kErr));
                progressTotal->setValue(i++);
            }
        }
    }

    MainWindow *mainWindow = (MainWindow *)parent();
    mainWindow->FillTableWidget(mainWindow->tabs[destPanel][destTab]->twFiles, mainWindow->tabs[destPanel][destTab]->lePath->text(), mainWindow->tabs[destPanel][destTab]->sortFlags);
    this->close();
}

int FileOperations::doCopyOneFile(QString &source, QString &destination)
{
    int result = kErrNone;

    if (!source.compare(destination))
        return kSameFile;

    QFile sourceFile(source);
    QFile destFile(destination);

    progressFile->setMaximum(sourceFile.size() / kCopyBufferSize);
    int i = 1;

    if (!sourceFile.open( QIODevice::ReadOnly ))
        return kCantOpenSource;

    if (!destFile.open( QIODevice::WriteOnly ))
    {
        sourceFile.close();
        return kCantOpenDestination;
    }

    char *buffer = new char[kCopyBufferSize];
    while (!sourceFile.atEnd())
    {
        qint64 len = sourceFile.read(buffer, kCopyBufferSize);
        len = destFile.write(buffer, len);
        if (-1 == len)
        {
            sourceFile.close();
            destFile.close();
            return kCosSieSpieprzyloPodczasKopiowania;
        }
        progressFile->setValue(i++);
    }

    sourceFile.close();
    destFile.close();

    return result;
}

void FileOperations::doFilesCount()
{
    for (QList<QString>::Iterator iterator = files->begin(); iterator != files->end(); iterator++)
    {
        if (QFileInfo(source + *iterator).isDir())
            filesToCopy += QDir(source + *iterator).count();
        else
            ++filesToCopy;
    }

    progressTotal->setMinimum(0);
    progressTotal->setMaximum(filesToCopy);
    progressTotal->setValue(0);
    progressFile->setMinimum(0);
}

void FileOperations::doMkDir(QString &/*path*/)
{

}

void FileOperations::draw()
{
    this->setLayout(mainLayout);
    //this->setWindowModality(Qt::WindowModal);
    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->show();
}

void FileOperations::drawCopyMoveFiles()
{
//    if (operationCount)
//        return;
//    ++operationCount;
    mainLayout = new QVBoxLayout();
    //przysicki: drzewo lub kolumny do wyboru celu
    lblSource = new QLabel(QObject::tr("Source:"));
    lblDestination = new QLabel(QObject::tr("Destination:"));
    leSource = new QLineEdit(source);
    leDestination = new QLineEdit(destination);
    buttons = new QWidget();
    buttonsLayout = new QHBoxLayout();
    pbOk = new QPushButton(QObject::tr("Ok"));
    pbCancel = new QPushButton(QObject::tr("Cancel"));

    buttonsLayout->addWidget(pbOk);
    buttonsLayout->addWidget(pbCancel);
    buttons->setLayout(buttonsLayout);

    mainLayout->addWidget(lblSource);
    mainLayout->addWidget(leSource);
    mainLayout->addWidget(lblDestination);
    mainLayout->addWidget(leDestination);
    mainLayout->addWidget(buttons);

    connect(pbOk, SIGNAL(clicked()), this, SLOT(okProceed()));
    connect(pbCancel, SIGNAL(clicked()), this, SLOT(cancel()));

    if (kCopy == operation)
        this->setWindowTitle(tr("Copy files"));
    else
        this->setWindowTitle(tr("Move files"));

    draw();
}

void FileOperations::drawCopyMoveProgress()
{
    mainLayout = new QVBoxLayout();
    progressTotal = new QProgressBar();
    progressFile = new QProgressBar();
    lblSource = new QLabel(QObject::tr("Source: ") );
    lblDestination = new QLabel(QObject::tr("Destination: "));
    buttons = new QWidget();
    buttonsLayout = new QHBoxLayout();
    spacer = new QSpacerItem(100,1);
    pbCancel = new QPushButton(QObject::tr("Cancel"));

    buttonsLayout->addItem(spacer);
    //buttonsLayout->addWidget(spacer);
    buttonsLayout->addWidget(pbCancel);
    buttons->setLayout(buttonsLayout);

    mainLayout->addWidget(lblSource);
    mainLayout->addWidget(lblDestination);
    mainLayout->addWidget(progressTotal);
    mainLayout->addWidget(progressFile);
    mainLayout->addWidget(buttons);

    draw();
}

void FileOperations::drawFileCopyErrorMessage(int error)
{
    QMessageBox::warning(this, tr("Copy failed!"), tr("Copy failed! Error code: %1").arg(error));
}

void FileOperations::drawMkDir()
{
    this->operation = kMkDir;
    mainLayout = new QVBoxLayout();

    lblSource = new QLabel(QObject::tr("Source:"));
    leSource = new QLineEdit(source);

    buttons = new QWidget();
    buttonsLayout = new QHBoxLayout();
    pbOk = new QPushButton(QObject::tr("Ok"));
    pbCancel = new QPushButton(QObject::tr("Cancel"));

    buttonsLayout->addWidget(pbOk);
    buttonsLayout->addWidget(pbCancel);
    buttons->setLayout(buttonsLayout);
    mainLayout->addWidget(lblSource);
    mainLayout->addWidget(leSource);
    mainLayout->addWidget(buttons);

    connect(pbOk, SIGNAL(clicked()), this, SLOT(okProceed()));
    connect(pbCancel, SIGNAL(clicked()), this, SLOT(cancel()));

    this->setWindowTitle(tr("Move files"));

    draw();
}

QString FileOperations::pathCombine(QString &first, QString &second)
{
    if (('/' == first.at(first.size() - 1) ) || ('\\' == first.at(first.size() - 1)))
        return first + second;
    else
        return first + "/" + second;
}
