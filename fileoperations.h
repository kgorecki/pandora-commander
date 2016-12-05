#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QDialog>
#include <QString>
#include <QList>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

enum eOperation
{
    kCopy = 0,
    kRenMove,
    kMkDir
};

enum eCopyError
{
    kErrNone = 0,
    kSameFile = -1,
    kCantOpenSource = -2,
    kCantOpenDestination = 3,
    kCosSieSpieprzyloPodczasKopiowania = -4,
    kFileAlreadyExists = -5,
    kCreateDirectoryFailed = -6
};

const uint kCopyBufferSize = 16384;
const int kOperationTypesCount = 2;

class FileOperations : public QDialog
{
    Q_OBJECT

public:
    FileOperations(QWidget *parent = 0);
    void CopyMoveFiles(QList<QString> *files, QString source, QString destination, int destPanel, int destTab, int operation);
    void MkDir(QString curDir);

private slots:
    void okProceed();
    void cancel();

private:
    void clearAll();
    void doCopyMoveDir(QString &path);
    void doCopyMoveFiles();
    int doCopyOneFile(QString &source, QString &destination);
    void doFilesCount();
    void doMkDir(QString &path);
    void draw();
    void drawCopyMoveFiles();
    void drawCopyMoveProgress();
    void drawFileCopyErrorMessage(int error);
    void drawMkDir();
    QString pathCombine(QString &first, QString &second);

    QVBoxLayout *mainLayout;
    //przysicki: drzewo lub kolumny do wyboru celu
    QLabel *lblSource;
    QLabel *lblDestination;
    QLineEdit *leSource;
    QLineEdit *leDestination;
    QWidget *buttons;
    QHBoxLayout *buttonsLayout;
    QPushButton *pbOk;
    QPushButton *pbCancel;
    QSpacerItem *spacer;
    QProgressBar *progressTotal;
    QProgressBar *progressFile;

    int operation;
    quint64 filesToCopy;
    int destPanel, destTab;
    //static int operationCount[kOperationTypesCount];
    static int operationCount;
    QList<QString> *files;
    QString source;
    QString destination;
};

#endif // FILEOPERATIONS_H
