/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#include "qtcopydialog.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <QtCore/QEvent>
#include <QMessageBox>
#include <QStyle>
#include "qtfilecopier.h"
#include "ui_qtcopydialog.h"
#include "ui_qtoverwritedialog.h"
#include "ui_qtotherdialog.h"

static QString formatSize(qint64 size)
{
    QString str = QtCopyDialog::tr("%1 B").arg(size);
    int base = 1024;
    qint64 mult = 1;
    for (int i = 0; i < 4; i++) {
        qint64 div = size / mult;
        if (div < base) {
            double val = (double)size / mult;
            switch (i) {
                case 1: str = QtCopyDialog::tr("%1 kB").arg(val, 0, 'f', 1); break;
                case 2: str = QtCopyDialog::tr("%1 MB").arg(val, 0, 'f', 1); break;
                case 3: str = QtCopyDialog::tr("%1 GB").arg(val, 0, 'f', 1); break;
                default: break;
            }
            break;
        }
        mult *= base;
    }
    return str;
}

///////////////////////////

class QtOverwriteDialog : public QDialog
{
    Q_OBJECT
public:
    QtOverwriteDialog(QWidget *parent = 0);

    enum ResultButton {
        Cancel,
        Skip,
        SkipAll,
        Overwrite,
        OverwriteAll
    };

    ResultButton execute(const QString &sourceFile, const QString &destinationFile);
private slots:
    void cancel()       { done(Cancel); }
    void skip()         { done(Skip); }
    void skipAll()      { done(SkipAll); }
    void overwrite()    { done(Overwrite); }
    void overwriteAll() { done(OverwriteAll); }
private:
    Ui::QtOverwriteDialog ui;
};

QtOverwriteDialog::QtOverwriteDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.skipButton, SIGNAL(clicked()), this, SLOT(skip()));
    connect(ui.skipAllButton, SIGNAL(clicked()), this, SLOT(skipAll()));
    connect(ui.overwriteButton, SIGNAL(clicked()), this, SLOT(overwrite()));
    connect(ui.overwriteAllButton, SIGNAL(clicked()), this, SLOT(overwriteAll()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    ui.iconLabel->setPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxWarning));
}

QtOverwriteDialog::ResultButton QtOverwriteDialog::execute(const QString &sourceFile,
        const QString &destinationFile)
{
    ui.sourceLineEdit->setText(sourceFile);
    ui.destLineEdit->setText(destinationFile);
    QFileInfo fis(sourceFile);
    QFileInfo fid(destinationFile);
    ui.sourceFileLabel->setText(formatSize(fis.size()));
    ui.destinationFileLabel->setText(formatSize(fid.size()));
    int result = exec();
    return (ResultButton)result;
}

class QtOtherDialog : public QDialog
{
    Q_OBJECT
public:
    QtOtherDialog(QWidget *parent = 0);

    enum ResultButton {
        Cancel,
        Skip,
        SkipAll,
        Retry
    };

    ResultButton execute(const QString &sourceFile, const QString &destinationFile,
                    const QString &title, const QString &message);
private slots:
    void cancel()       { done(Cancel); }
    void skip()         { done(Skip); }
    void skipAll()      { done(SkipAll); }
    void retry()        { done(Retry); }
private:
    Ui::QtOtherDialog ui;
};

QtOtherDialog::QtOtherDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.skipButton, SIGNAL(clicked()), this, SLOT(skip()));
    connect(ui.skipAllButton, SIGNAL(clicked()), this, SLOT(skipAll()));
    connect(ui.retryButton, SIGNAL(clicked()), this, SLOT(retry()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    ui.iconLabel->setPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxWarning));
}

QtOtherDialog::ResultButton QtOtherDialog::execute(const QString &sourceFile,
        const QString &destinationFile, const QString &title, const QString &message)
{
    ui.sourceLineEdit->setText(sourceFile);
    ui.destLineEdit->setText(destinationFile);
    ui.messageLabel->setText(message);
    setWindowTitle(title);
    int result = exec();
    return (ResultButton)result;
}

////////////////////////////////////////////////

class QtCopyDialogPrivate {
    QtCopyDialog *q_ptr;
    Q_DECLARE_PUBLIC(QtCopyDialog)
public:
    QtCopyDialogPrivate() {}

    void init();

    void error(int id, QtFileCopier::Error error, bool stopped);
    void stateChanged(QtFileCopier::State state);
    void done(bool error);
    void started(int id);
    void dataTransferProgress(int id, qint64 progress);
    void finished(int id, bool error);
    void canceled();
    void childrenCanceled(int id);

    void showProgress();
    void showDialog();
    void reset();
    void setFileLabel(int currentF, int totalFiles);
    void setDirLabel(int currentD, int totalDirs);
    void setFileNames(const QString &source, const QString &dest);
    void setCurrentProgress(qint64 completed, qint64 totalSize);
    void setCompleted(qint64 completed, qint64 totalSize, int msecs);

    void addRequest(int id);

    struct Request {
        QString source;
        QString dest;
        qint64 size;
    };

    QtFileCopier *fileCopier;
    bool autoClose;
    QTimer *showTimer;
    QTime startTime;

    QMap<int, Request> requests;
    int currentFile;
    qint64 totalSize;
    qint64 currentProgress;
    qint64 currentDone;
    int currentProgressTime;
    int currentDoneTime;
    int dirCount;
    int currentDir;

    int lastCurrentId;
    Ui::QtCopyDialog ui;
};

void QtCopyDialogPrivate::init()
{
    Q_Q(QtCopyDialog);
    ui.setupUi(q);
    qRegisterMetaType<qint64>("qint64");

    fileCopier = 0;

    autoClose = false;

    currentFile = 0;
    totalSize = 0;
    currentProgress = 0;
    currentDone = 0;
    currentProgressTime = 0;
    currentDoneTime = 0;
    dirCount = 0;
    currentDir = 0;
    lastCurrentId = -1;

    showTimer = new QTimer(q);
    showTimer->setInterval(2000);
    showTimer->setSingleShot(true);
    q->connect(showTimer, SIGNAL(timeout()),
                q, SLOT(showDialog()));

}

void QtCopyDialogPrivate::error(int id, QtFileCopier::Error error, bool stopped)
{
    Request r = requests[id];
    if (!stopped)
        return;
    showProgress();
    showDialog();

    QString title;
    QString text;
    bool handled = false;
    switch (error) {
        case QtFileCopier::SourceNotExists: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Source doesn't exist.");
            break;
        }
        case QtFileCopier::DestinationExists: {
            QtOverwriteDialog mb(q_ptr);
            switch (mb.execute(r.source, r.dest)) {
                case QtOverwriteDialog::Cancel       : fileCopier->cancelAll();
                                                       fileCopier->retry();        break;
                case QtOverwriteDialog::Skip         : fileCopier->skip();         break;
                case QtOverwriteDialog::SkipAll      : fileCopier->skipAll();      break;
                case QtOverwriteDialog::Overwrite    : fileCopier->overwrite();    break;
                case QtOverwriteDialog::OverwriteAll : fileCopier->overwriteAll(); break;
                default                              : fileCopier->retry();        break;
            }
            handled = true;
            break;
        }
        case QtFileCopier::SourceDirectoryOmitted: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Source file is a directory. Omitting source directory.");
            break;
        }
        case QtFileCopier::SourceFileOmitted: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Source directory is a file. Omitting source file.");
            break;
        }
        case QtFileCopier::PathToDestinationNotExists: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Path to destination does not exist.");
            break;
        }
        case QtFileCopier::CannotCreateDestinationDirectory: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Cannot create destination directory.");
            break;
        }
        case QtFileCopier::CannotOpenSourceFile: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Cannot open source file. Please check permissions.");
            break;
        }
        case QtFileCopier::CannotOpenDestinationFile: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Cannot open destination file. Please check permissions.");
            break;
        }
        case QtFileCopier::CannotRemoveDestinationFile: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Force: Cannot remove destination file.");
            break;
        }
        case QtFileCopier::CannotCreateSymLink: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Cannot create symbolic link targeting to source.");
            break;
        }
        case QtFileCopier::CannotReadSourceFile: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Cannot read source file.");
            break;
        }
        case QtFileCopier::CannotWriteDestinationFile: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Cannot write destination file.");
            break;
        }
        case QtFileCopier::CannotRemoveSource: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Cannot remove source.");
            break;
        }
        default: {
            title = q_ptr->tr("Copy Warning");
            text = q_ptr->tr("Error code: %1").arg(error);
            break;
        }
    }
    if (!handled) {
        QtOtherDialog mb(q_ptr);
        switch (mb.execute(r.source, r.dest, title, text)) {
            case QtOtherDialog::Cancel  : fileCopier->cancelAll();
                                          fileCopier->retry();   break;
            case QtOtherDialog::Skip    : fileCopier->skip();    break;
            case QtOtherDialog::SkipAll : fileCopier->skipAll(); break;
            case QtOtherDialog::Retry   : fileCopier->retry();   break;
            default                     : fileCopier->retry();   break;
        }
    }
}

void QtCopyDialogPrivate::stateChanged(QtFileCopier::State state)
{
    Q_Q(QtCopyDialog);
    if (state == QtFileCopier::Busy) {
        if (fileCopier->state() == QtFileCopier::Idle) {
            reset();
            showProgress();
            showTimer->start();
        }
        startTime.start();
        ui.cancelButton->setEnabled(true);
        ui.closeButton->setEnabled(false);
    } else if (state == QtFileCopier::Idle) {
        showTimer->stop();
        showProgress();
        QString str = QtCopyDialog::tr("100% of %1 (Done)").arg(formatSize(totalSize));
        q->setWindowTitle(str);
        ui.cancelButton->setEnabled(false);
        ui.closeButton->setEnabled(true);
    } else {
        showTimer->stop();
        ui.cancelButton->setEnabled(true);
        ui.closeButton->setEnabled(false);
    }
}

void QtCopyDialogPrivate::done(bool /*error*/)
{
    Q_Q(QtCopyDialog);
    if (autoClose)
        q->accept();
}

void QtCopyDialogPrivate::started(int id)
{
    startTime.start();
    lastCurrentId = id;
    currentProgress = 0;
    currentProgressTime = 0;
    QFileInfo fi(requests[id].source);
    qint64 size = fi.isDir() ? 0 : fi.size();
    if (requests[id].size != size) {
        totalSize -= requests[id].size;
        totalSize += size;
        requests[id].size = size;
    }
    QTimer::singleShot(0, q_ptr, SLOT(showProgress()));
}

void QtCopyDialogPrivate::dataTransferProgress(int id, qint64 progress)
{
    Q_ASSERT(lastCurrentId == id);
    Q_UNUSED(id);
    currentProgressTime = startTime.elapsed();
    currentProgress = progress;
    QTimer::singleShot(0, q_ptr, SLOT(showProgress()));
}

void QtCopyDialogPrivate::finished(int id, bool error)
{
    currentFile++;
    if (fileCopier->isDir(id))
        currentDir++;
    totalSize -= requests[id].size;
    if (!error) {
        currentDoneTime += currentProgressTime;
        currentDone += currentProgress;
        totalSize += currentProgress;
    } else {
        childrenCanceled(id);
    }
    lastCurrentId = fileCopier->currentId();
    if (lastCurrentId < 0)
        lastCurrentId = id;
    currentProgressTime = 0;
    currentProgress = 0;
    QTimer::singleShot(0, q_ptr, SLOT(showProgress()));
}

void QtCopyDialogPrivate::canceled()
{
    totalSize = currentDone;
    currentDir = dirCount;
    currentFile = requests.size() - dirCount;
    showProgress();
}

void QtCopyDialogPrivate::childrenCanceled(int id)
{
    if (!fileCopier->isDir(id))
        return;
    QList<int> children = fileCopier->entryList(id);
    QListIterator<int> itChild(children);
    while (itChild.hasNext()) {
        int childId = itChild.next();
        currentFile++;
        totalSize -= requests[childId].size;
        if (fileCopier->isDir(childId)) {
            currentDir++;
            childrenCanceled(childId);
        }
    }
}

void QtCopyDialogPrivate::showProgress()
{
    qint64 completed = currentDone + currentProgress;
    setFileLabel(currentFile - currentDir, requests.size() - dirCount);
    setDirLabel(currentDir, dirCount);
    setCompleted(completed, totalSize, currentDoneTime + currentProgressTime);
    if (lastCurrentId == -1) {
        setCurrentProgress(0, 1);
        setFileNames(QString(), QString());
    } else {
        if (fileCopier->currentId() == -1)
            setCurrentProgress(1, 1);
        else
            setCurrentProgress(currentProgress, requests[lastCurrentId].size);
        setFileNames(requests[lastCurrentId].source, requests[lastCurrentId].dest);
    }
}

void QtCopyDialogPrivate::showDialog()
{
    Q_Q(QtCopyDialog);
    q->show();
    q->raise();
}

void QtCopyDialogPrivate::reset()
{
    currentFile = 0;
    totalSize = 0;
    currentProgress = 0;
    currentDone = 0;
    currentProgressTime = 0;
    currentDoneTime = 0;
    dirCount = 0;
    currentDir = 0;
    lastCurrentId = -1;
    requests.clear();
    if (fileCopier) {
        QList<int> idList = fileCopier->pendingRequests();
        QListIterator<int> itId(idList);
        while (itId.hasNext()) {
            addRequest(itId.next());
        }
    }

    showProgress();
}

void QtCopyDialogPrivate::setFileLabel(int currentF, int totalFiles)
{
    QString str = QtCopyDialog::tr("%1 / %2 files").arg(currentF).arg(totalFiles);
    ui.filesLabel->setText(str);
}

void QtCopyDialogPrivate::setDirLabel(int currentD, int totalDirs)
{
    QString str;
    if (totalDirs > 0)
        str = QtCopyDialog::tr("%1 / %2 dirs").arg(currentD).arg(totalDirs);
    ui.dirsLabel->setText(str);
}

void QtCopyDialogPrivate::setFileNames(const QString &source, const QString &dest)
{
    ui.sourceLineEdit->setText(source);
    ui.destLineEdit->setText(dest);
}

void QtCopyDialogPrivate::setCurrentProgress(qint64 completed, qint64 totalSize)
{
    int percent = totalSize > 0 ? (int)((double)completed * 100 / totalSize + 0.5) : 0;
    ui.currentProgressBar->setValue(percent);
}

void QtCopyDialogPrivate::setCompleted(qint64 completed, qint64 totalSize, int msecs)
{
    QString str = QtCopyDialog::tr("%1 of %2 completed").arg(formatSize(completed))
                .arg(formatSize(totalSize));
    ui.completedLabel->setText(str);
    int percent = totalSize > 0 ? (int)((double)completed * 100 / totalSize + 0.5) : 100;
    ui.totalProgressBar->setValue(percent);
    QString state;
    if (fileCopier && fileCopier->state() != QtFileCopier::Idle)
        state = QtCopyDialog::tr("Copying...");
    else
        state = QtCopyDialog::tr("Done");
    str = QtCopyDialog::tr("%1% of %2 (%3)").arg(percent)
                .arg(formatSize(totalSize)).arg(state);
    q_ptr->setWindowTitle(str);

    QString transferStr = QtCopyDialog::tr("0 B");
    if (msecs > 0) {
        int transfer = (int)((double)completed * 1000 / msecs);
        transferStr = formatSize(transfer);
    }
    QString estStr("00:00:00");
    if (completed > 0) {
        QTime est;
        int estMSecs = (int)((double)totalSize * msecs / completed - msecs + 0.5);
        est = est.addMSecs(estMSecs);
        estStr = est.toString("hh:mm:ss");
    }
    str = QtCopyDialog::tr("%1/s ( %2 remaining )").arg(transferStr).arg(estStr);
    ui.remainingLabel->setText(str);
}

void QtCopyDialogPrivate::addRequest(int id)
{
    Request r;
    r.source = fileCopier->sourceFilePath(id);
    r.dest = fileCopier->destinationFilePath(id);
    QFileInfo fis(r.source);
    r.size = fis.size();
    if (fileCopier->isDir(id)) {
        r.size = 0;
        dirCount++;
    }
    requests[id] = r;

    totalSize += r.size;
}

/*! \class QtCopyDialog

    \brief The QtCopyDialog class provides visual feedback for large
    copy and move operations, and it provides the option of
    aborting an operation.

    \image qtcopydialog.png

    QtCopyDialog is used to give the user an indication of how much
    time a copy or move operation is going to take. It estimates and
    displays the time necessary to complete the operation. It also gives
    the user the option of aborting the operation at any time.

    QtCopyDialog is normally used with the QtFileCopier class which
    perform copy and move operations on files in a background
    process. To connect the copy dialog to an QtFileCopier instance,
    you can use the setFileCopier() function.

    The behavior of the copy dialog can be controlled using its
    autoClose and minimumDuration properties: The autoClose property
    holds whether the dialog should be automatically closed when the
    operation is completed. The minimumDuration property specifies the
    time that must pass before the dialog appears.

    \sa QtFileCopier
*/

/*!
    \fn QtCopyDialog::QtCopyDialog(QWidget *parent)

    Creates a copy dialog with the given \a parent and the specified
    window \a flags.
*/

QtCopyDialog::QtCopyDialog(QWidget *parent)
    : QDialog(parent)
{
    d_ptr = new QtCopyDialogPrivate;
    Q_D(QtCopyDialog);
    d->q_ptr = this;
    d->init();

    //setModal(false);
}

/*!
    \fn QtCopyDialog::QtCopyDialog(QtFileCopier *copier, QWidget *parent)

    Creates a copy dialog with the given \a parent and the specified
    window \a flags.

    The dialog is connected to the specified file \a copier. The file
    copier's current state must be QtFileCopier::Idle, i.e. it cannot
    be performing any operation and its pending list must be empty.

    \sa setFileCopier()
*/

QtCopyDialog::QtCopyDialog(QtFileCopier *copier, QWidget *parent)
    : QDialog(parent)
{
    d_ptr = new QtCopyDialogPrivate;
    Q_D(QtCopyDialog);
    d->q_ptr = this;
    d->init();

    setFileCopier(copier);

    //setModal(false);
}

/*!
    Destroys the copy dialog.
*/

QtCopyDialog::~QtCopyDialog()
{
    Q_D(QtCopyDialog);
    delete d;
}

/*!
    Connects the copy dialog to the given file \a copier.  The file
    copier's current state must be QtFileCopier::Idle, i.e. it cannot
    be performing any operation and its pending list must be empty.

    \sa fileCopier()
*/

void QtCopyDialog::setFileCopier(QtFileCopier *copier)
{
    Q_D(QtCopyDialog);
    if (d->fileCopier == copier)
        return;

    if (copier && copier->state() != QtFileCopier::Idle) {
        // can't start track copier while not idle
        return;
    }

    if (d->fileCopier) {
        disconnect(d->fileCopier, SIGNAL(error(int, QtFileCopier::Error, bool)),
                this, SLOT(error(int, QtFileCopier::Error, bool)));
        disconnect(d->fileCopier, SIGNAL(stateChanged(QtFileCopier::State)),
                this, SLOT(stateChanged(QtFileCopier::State)));
        disconnect(d->fileCopier, SIGNAL(started(int)),
                this, SLOT(started(int)));
        disconnect(d->fileCopier, SIGNAL(done(bool)),
                this, SLOT(done(bool)));
        disconnect(d->fileCopier, SIGNAL(dataTransferProgress(int, qint64)),
                this, SLOT(dataTransferProgress(int, qint64)));
        disconnect(d->fileCopier, SIGNAL(finished(int, bool)),
                this, SLOT(finished(int, bool)));
        disconnect(d->fileCopier, SIGNAL(canceled()),
                this, SLOT(canceled()));
    }

    d->fileCopier = copier;

    if (d->fileCopier) {
        connect(d->fileCopier, SIGNAL(error(int, QtFileCopier::Error, bool)),
                this, SLOT(error(int, QtFileCopier::Error, bool)));
        connect(d->fileCopier, SIGNAL(stateChanged(QtFileCopier::State)),
                this, SLOT(stateChanged(QtFileCopier::State)));
        connect(d->fileCopier, SIGNAL(started(int)),
                this, SLOT(started(int)));
        connect(d->fileCopier, SIGNAL(done(bool)),
                this, SLOT(done(bool)));
        connect(d->fileCopier, SIGNAL(dataTransferProgress(int, qint64)),
                this, SLOT(dataTransferProgress(int, qint64)));
        connect(d->fileCopier, SIGNAL(finished(int, bool)),
                this, SLOT(finished(int, bool)));
        connect(d->fileCopier, SIGNAL(canceled()),
                this, SLOT(canceled()));
    }
}

/*!
   Returns the file copier currently connected to the copy dialog.

   \sa setFileCopier()
*/

QtFileCopier *QtCopyDialog::fileCopier() const
{
    Q_D(const QtCopyDialog);
    return d->fileCopier;
}


/*!
    \property QtCopyDialog::minimumDuration
    \brief the time that must pass before the dialog appears.

    When all pending operations are completed within the specified
    time frame, the dialog will not appear at all. If the minimum
    duration is set to 0, the dialog appears as soon as the program
    execution returns to the event processing loop.

    The default duration is 2000 miliseconds.
*/

int QtCopyDialog::minimumDuration() const
{
    Q_D(const QtCopyDialog);
    return d->showTimer->interval();
}

void QtCopyDialog::setMinimumDuration(int ms)
{
    Q_D(QtCopyDialog);
    d->showTimer->setInterval(ms);
}

/*!
    \property QtCopyDialog::autoClose
    \brief whether the dialog should be automatically closed when the
    operation is completed.

    The default is true.
*/

/*!
    \fn void QtCopyDialog::setAutoClose(bool on)
*/
void QtCopyDialog::setAutoClose(bool b)
{
    Q_D(QtCopyDialog);
    d->autoClose = b;
}

bool QtCopyDialog::autoClose() const
{
    Q_D(const QtCopyDialog);
    return d->autoClose;
}

/*!
    \reimp
*/

void QtCopyDialog::reject()
{
    Q_D(QtCopyDialog);
    if (d->fileCopier && d->fileCopier->state() != QtFileCopier::Idle)
        d->fileCopier->cancelAll();
    QDialog::reject();
}

#include "qtcopydialog.moc"
#include "moc_qtcopydialog.cpp"
