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

#include "qtfilecopier.h"
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QQueue>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QStack>
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QMetaType>

struct CopyRequest {
    CopyRequest() {
        move = false;
        dir = false;
    }
    QQueue<int> childrenQueue;
    QString source;
    QString dest;
    bool move;
    bool dir;
    QtFileCopier::CopyFlags copyFlags;
};

class QtCopyThread : public QThread
{
    Q_OBJECT
public:
    QtCopyThread(QtFileCopier *fileCopier);
    ~QtCopyThread();

    struct Request {
        Request() {
            canceled = false;
            overwrite = false;
            moveError = false;
        }
        Request(const CopyRequest &r) {
            request = r;
            canceled = false;
            overwrite = false;
            moveError = false;
        }
        CopyRequest request;
        bool canceled;
        bool overwrite;
        bool moveError;
    };

    void emitProgress(int id, qint64 progress) {
        QMutexLocker l(&mutex);
        emit dataTransferProgress(id, progress);
        progressRequest = 0;
    }
    bool isCanceled(int id) const {
        QMutexLocker l(&mutex);
        if (cancelRequest)
            return true;
        if (requestQueue.empty())
            return false;
        return requestQueue[id].canceled;
    }
    bool isMoveError(int id) const {
        QMutexLocker l(&mutex);
        if (requestQueue.empty())
            return false;
        return requestQueue[id].moveError;
    }
    bool isProgressRequest() const {
        return (progressRequest != 0);
    }
    void setMoveError(int id, bool error) {
        QMutexLocker l(&mutex);
        if (requestQueue.empty())
            return;
        requestQueue[id].moveError = error;
    }
    void handle(int id);
    void lockCancelChildren(int id);
    void renameChildren(int id);
    void cancelChildRequests(int id);
    void overwriteChildRequests(int id);

    void setAutoReset(bool on);
public slots:
    void restart();

    void copy(int id, const CopyRequest &request);
    void copy(const QMap<int, CopyRequest> &requests);

    void cancel();
    void cancel(int id);

    void skip();
    void skipAll();
    void retry();

    void overwrite();
    void overwriteAll();

    void resetOverwrite();
    void resetSkip();

    void progress();
signals:
    void error(int id, QtFileCopier::Error error, bool stopped);
    void started(int id);
    void dataTransferProgress(int id, qint64 progress);
    void finished(int id, bool error);
    void canceled();
protected:
    void run();
protected slots:
    void copierDestroyed();
private:

    void cancelChildren(int id);

    QtFileCopier *copier;
    QMap<int, Request> requestQueue;
    mutable QMutex mutex;
    QWaitCondition newCopyCondition;
    QWaitCondition interactionCondition;
    bool waitingForInteraction;
    bool stopRequest;
    bool skipAllRequest;
    QSet<QtFileCopier::Error> skipAllError;
    bool overwriteAllRequest;
    bool cancelRequest;
    int currentId;
#if QT_VERSION >= 0x040400
    QAtomicInt progressRequest;
#else
    QAtomic progressRequest;
#endif
    bool autoReset;
};

QtCopyThread::QtCopyThread(QtFileCopier *fileCopier)
    : QThread(QCoreApplication::instance()),
      copier(fileCopier),
      waitingForInteraction(false),
      stopRequest(false),
      skipAllRequest(false),
      overwriteAllRequest(false),
      cancelRequest(false),
      currentId(-1),
      autoReset(true)
{
    qRegisterMetaType<QtFileCopier::Error>("QtFileCopier::Error");
    connect(this, SIGNAL(error(int, QtFileCopier::Error, bool)),
                copier, SLOT(copyError(int, QtFileCopier::Error, bool)));
    connect(this, SIGNAL(started(int)),
                copier, SLOT(copyStarted(int)));
    connect(this, SIGNAL(dataTransferProgress(int, qint64)),
                copier, SIGNAL(dataTransferProgress(int, qint64)));
    connect(this, SIGNAL(finished(int, bool)),
                copier, SLOT(copyFinished(int, bool)));
    connect(this, SIGNAL(canceled()),
                copier, SLOT(copyCanceled()));
    connect(copier, SIGNAL(destroyed()),
                this, SLOT(copierDestroyed()));
}

QtCopyThread::~QtCopyThread()
{
    if (isRunning()) {
        wait();
    }
}

void QtCopyThread::copierDestroyed()
{
    QMutexLocker l(&mutex);
    stopRequest = true;
    newCopyCondition.wakeOne();
    interactionCondition.wakeOne();
}

void QtCopyThread::restart()
{
    start();
    newCopyCondition.wakeOne();
}

void QtCopyThread::copy(int id, const CopyRequest &request)
{
    Request r(request);
    QMutexLocker l(&mutex);
    requestQueue[id] = r;
//    newCopyCondition.wakeOne();
}

void QtCopyThread::copy(const QMap<int, CopyRequest> &requests)
{
    QMap<int, CopyRequest>::ConstIterator it = requests.constBegin();
    QMutexLocker l(&mutex);
    while (it != requests.constEnd()) {
        Request r(it.value());
        requestQueue[it.key()] = r;
        it++;
    }
//    newCopyCondition.wakeOne();
}

void QtCopyThread::cancelChildRequests(int id)
{
    QMap<int, Request>::Iterator it = requestQueue.find(id);
    if (it != requestQueue.end()) {
        Request &r = it.value();
        r.canceled = true;
        QListIterator<int> itChild(r.request.childrenQueue);
        while (itChild.hasNext())
            cancelChildRequests(itChild.next());
    }
}

void QtCopyThread::overwriteChildRequests(int id)
{
    QMap<int, Request>::Iterator it = requestQueue.find(id);
    if (it != requestQueue.end()) {
        Request &r = it.value();
        r.overwrite = true;
        QListIterator<int> itChild(r.request.childrenQueue);
        while (itChild.hasNext())
            overwriteChildRequests(itChild.next());
    }
}

void QtCopyThread::cancel()
{
    QMutexLocker l(&mutex);
    QMutableMapIterator<int, Request> it(requestQueue);
    while (it.hasNext())
        it.next().value().canceled = true;
    cancelRequest = true;
    /*
    // if waitingForInteraction is true wake must be done by retry other method.
    if (waitingForInteraction)
        interactionCondition.wakeOne();
    */
}

void QtCopyThread::cancel(int id)
{
    QMutexLocker l(&mutex);
    cancelChildRequests(id);
    /*
    if (waitingForInteraction && currentId == id)
        interactionCondition.wakeOne();
    */
}

void QtCopyThread::skip()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    cancelChildRequests(currentId);
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void QtCopyThread::skipAll()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    cancelChildRequests(currentId);
    skipAllRequest = true;
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void QtCopyThread::overwrite()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    overwriteChildRequests(currentId);
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void QtCopyThread::overwriteAll()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    overwriteAllRequest = true;
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void QtCopyThread::retry()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void QtCopyThread::resetOverwrite()
{
    QMutexLocker l(&mutex);
    overwriteAllRequest = true;
}

void QtCopyThread::resetSkip()
{
    QMutexLocker l(&mutex);
    skipAllError.clear();
}

void QtCopyThread::setAutoReset(bool on)
{
    QMutexLocker l(&mutex);
    autoReset = on;
}

void QtCopyThread::progress()
{
    progressRequest = 1;
}

struct ChainNode {
    ChainNode(ChainNode *nextInChain) {
        n = nextInChain;
        err = QtFileCopier::NoError;
    }
    virtual ~ChainNode() {
        if (n)
            delete n;
    }
    QtFileCopier::Error error() const {
        if (n)
            return n->error();
        return err;
    }
    virtual bool handle() {
        if (n)
            return n->handle();
        return false;
    }
protected:
    virtual CopyRequest &request() {
        return n->request();
    }
    virtual QtCopyThread *thread() const {
        if (n)
            return n->thread();
        return 0;
    }
    virtual int currentId() const {
        if (n)
            return n->currentId();
        return -1;
    }
    void setError(QtFileCopier::Error e) {
        if (n)
            n->setError(e);
        else
            err = e;
    }
private:
    ChainNode *n;
    QtFileCopier::Error err;
};

struct CanceledNode : public ChainNode {
    CanceledNode(ChainNode *nextInChain, bool canceled) : ChainNode(nextInChain) {
        c = canceled;
    }
    bool handle() {
        if (c) {
            CopyRequest &r = request();
            if (r.dir) {
                thread()->lockCancelChildren(currentId());
                //while (!r.childrenQueue.isEmpty())
                //    thread()->cancelChildren(r.childrenQueue.dequeue());
            }

            setError(QtFileCopier::Canceled);
            return true;
        }
        return ChainNode::handle();
    }
private:
    bool c;
};

struct SourceExistsNode : public ChainNode {
    SourceExistsNode(ChainNode *nextInChain) : ChainNode(nextInChain) {}
    bool handle() {
        CopyRequest &r = request();
        QFileInfo fis(r.source);
        if (!fis.exists() && !fis.isSymLink()) {
            setError(QtFileCopier::SourceNotExists);
            return false;
        }
        return ChainNode::handle();
    }
};

struct OverwriteNode : public ChainNode {
    OverwriteNode(ChainNode *nextInChain, bool overwrite) : ChainNode(nextInChain) {
        o = overwrite;
    }
    bool handle() {
        CopyRequest &r = request();
        QFileInfo fid(r.dest);
        bool overwrite = r.copyFlags & QtFileCopier::NonInteractive ?
                true : // true is default for non interactive mode
                o;
        if ((fid.exists() || fid.isSymLink()) && overwrite == false) {
            setError(QtFileCopier::DestinationExists);
            return false;
        }
#if 0
        if (!fis.isDir() || fis.isSymLink()) {
            QFileInfo fid(dest());
            if ((fid.exists() || fid.isSymLink()) && o == false) {
                setError(QtFileCopier::DestinationFileExists);
                return false;
            }
        }
#endif
        return ChainNode::handle();
    }
private:
    bool o;
};

struct MakeLinksNode : public ChainNode {
    MakeLinksNode(ChainNode *nextInChain) : ChainNode(nextInChain) { }
    bool handle() {
        CopyRequest &r = request();
        if (r.copyFlags & QtFileCopier::MakeLinks) {
            QFileInfo fis(r.source);
            QFileInfo fid(r.dest);
            QDir dir = fid.dir();
#if defined(Q_OS_WIN32)
            QString linkName = fis.absoluteFilePath();
#else
            QString linkName = dir.relativeFilePath(fis.filePath());
#endif
            QFile sourceFile(linkName);
            if (sourceFile.link(r.dest))
                return true;
            setError(QtFileCopier::CannotCreateSymLink);
            return false;
        }
        return ChainNode::handle();
    }
};

struct FollowLinksNode : public ChainNode {
    FollowLinksNode(ChainNode *nextInChain) : ChainNode(nextInChain) { }
    bool handle() {
        CopyRequest &r = request();
        QFileInfo fis(r.source);
        if (fis.isSymLink() && !(r.copyFlags & QtFileCopier::FollowLinks)) {
            QFileInfo fil(fis.readLink());
            QString linkName = fil.filePath();
#if defined(Q_OS_WIN32)
	    linkName = fil.absoluteFilePath();
#else
            if (fil.isAbsolute()) {
                QDir dir = fis.dir();
                linkName = dir.relativeFilePath(linkName);
            }
#endif
            QFile linkTarget(linkName);
            if (linkTarget.link(r.dest))
                return true;
            setError(QtFileCopier::CannotCreateSymLink);
            return false;
        }
        return ChainNode::handle();
    }
};

struct RenameNode : public ChainNode {
    RenameNode(ChainNode *nextInChain) : ChainNode(nextInChain) { }
    bool handle() {
        CopyRequest &r = request();
        if (r.move) {
            QFileInfo fis(r.source);
            QDir dir = fis.dir();
            if (!(r.copyFlags & QtFileCopier::FollowLinks) || !r.dir && !fis.isSymLink()) {
                if (dir.rename(fis.fileName(), r.dest)) {
                    QFileInfo fid(r.dest);
                    if (r.dir)
                        while (!r.childrenQueue.isEmpty())
                            thread()->renameChildren(r.childrenQueue.dequeue());
                    else
                        thread()->emitProgress(currentId(), fid.size());
                    return true;
                }
            }
        }
        return ChainNode::handle();
    }
};

struct MoveNode : public ChainNode {
    MoveNode(ChainNode *nextInChain) : ChainNode(nextInChain) { }
    bool handle() {
        CopyRequest &r = request();
        bool done = true;
        if (!thread()->isMoveError(currentId()))
            done = ChainNode::handle();
        if (done && error() == QtFileCopier::NoError && r.move) {
            bool moveError = false;
            QFileInfo fis(r.source);
            QDir sourceDir = fis.dir();
            if (fis.isDir() && !fis.isSymLink()) {
                if (!sourceDir.rmdir(fis.fileName()))
                    moveError = true;
            } else if (!sourceDir.remove(fis.fileName()))
                moveError = true;

            thread()->setMoveError(currentId(), moveError);
            if (moveError) {
                setError(QtFileCopier::CannotRemoveSource);
                done = false;
            }
        }
        return done;
    }
};

struct CopyDirNode : public ChainNode {
    CopyDirNode(ChainNode *nextInChain)
        : ChainNode(nextInChain) {
        }
    bool handle() {
        CopyRequest &r = request();
        if (!r.dir)
            return ChainNode::handle();
        QFileInfo fis(r.source);
        if (!fis.isDir()) {
            setError(QtFileCopier::SourceFileOmitted);
            return false;
        }
        QFileInfo fid(r.dest);
        if (!fid.exists()) {
            QDir destDir = fid.dir();
            if (!destDir.exists()) {
                setError(QtFileCopier::PathToDestinationNotExists);
                return false;
            } else if (!destDir.mkdir(fid.fileName())) {
                setError(QtFileCopier::CannotCreateDestinationDirectory);
                return false;
            }
        } else if (fid.isSymLink() || fid.isFile()) {
            setError(QtFileCopier::CannotCreateDestinationDirectory);
            return false;
        }

        //if (fid.isDir())
        while (!r.childrenQueue.isEmpty())
            thread()->handle(r.childrenQueue.dequeue());
        if (thread()->isCanceled(currentId()))
            setError(QtFileCopier::Canceled); // canceled
        return true;
    }
};

struct CopyFileNode : public ChainNode {
    CopyFileNode(ChainNode *nextInChain, int currentId, const CopyRequest &request,
                QtCopyThread *thread)
        : ChainNode(nextInChain) {
        id = currentId;
        r = request;
        t = thread;
    }
    CopyRequest &request() {
        return r;
    }
    QtCopyThread *thread() const {
        return t;
    }
    int currentId() const {
        return id;
    }
    bool handle() {
        CopyRequest &r = request();
        if (r.dir) {
            setError(QtFileCopier::SourceDirectoryOmitted);
            return false;
        }
        QFile sourceFile(r.source);
        QFile destFile(r.dest);
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            setError(QtFileCopier::CannotOpenSourceFile); // cannot open source file
            return false;
        }
        if (!destFile.open(QIODevice::WriteOnly)) {
            bool done = false;
            if (r.copyFlags & QtFileCopier::Force) {
                QFileInfo fid(r.dest);
                QDir dir = fid.dir();
                if (!dir.remove(fid.fileName()))
                    setError(QtFileCopier::CannotRemoveDestinationFile);
                else if (!destFile.open(QIODevice::WriteOnly))
                    setError(QtFileCopier::CannotOpenDestinationFile);
                else
                    done = true;
            } else
                setError(QtFileCopier::CannotOpenDestinationFile); // cannot open dest file

            if (!done) {
                sourceFile.close();
                return false;
            }
        }
        qint64 progress = 0;
        char block[4096];
        bool done = false;
        while (1) {
            if (t->isCanceled(id)) {
                setError(QtFileCopier::Canceled); // canceled
                done = true;
                break;
            }
            qint64 in = sourceFile.read(block, 4096);
            if (in == 0) {
                t->emitProgress(id, progress);
                break;
            }
            if (in == -1) {
                setError(QtFileCopier::CannotReadSourceFile); // cannot read
                break;
            }
            if (in != destFile.write(block, in)) {
                setError(QtFileCopier::CannotWriteDestinationFile); // cannot write
                break;
            }
            progress += in;
            if (t->isProgressRequest())
                t->emitProgress(id, progress);
        }
        destFile.close();
        sourceFile.close();
        if (error() != QtFileCopier::NoError)
            destFile.remove();
        else {
            destFile.setPermissions(sourceFile.permissions());
            done = true;
        }
        return done;
    }
private:
    CopyRequest r;
    QtCopyThread *t;
    int id;
};

void QtCopyThread::renameChildren(int id)
{
    mutex.lock();
    QMap<int, Request>::ConstIterator it = requestQueue.find(id);
    CopyRequest r = it.value().request;
    int oldCurrentId = currentId;
    currentId = it.key();
    mutex.unlock();
    emit started(id);

    while (!r.childrenQueue.isEmpty())
        renameChildren(r.childrenQueue.dequeue());

    if (!r.dir) {
        QFileInfo fid(r.dest);
        emitProgress(id, fid.size());
    }

    emit finished(id, false);
    mutex.lock();
    currentId = oldCurrentId;
    requestQueue.remove(id);
    mutex.unlock();
}

void QtCopyThread::lockCancelChildren(int id)
{
    QMutexLocker l(&mutex);
    cancelChildren(id);
}

void QtCopyThread::cancelChildren(int id)
{
//    mutex.lock();
    QMap<int, Request>::ConstIterator it = requestQueue.find(id);
    if (it == requestQueue.constEnd()) {
//        mutex.unlock();
        return;
    }
    CopyRequest r = it.value().request;
//    int oldCurrentId = currentId;
//    currentId = it.key();
//    mutex.unlock();
//    emit started(id);

    while (!r.childrenQueue.isEmpty()) {
        int childId = r.childrenQueue.dequeue();
        cancelChildren(childId);

        requestQueue.remove(childId);
    }

//    emit error(id, QtFileCopier::Canceled, false);

//    emit finished(id, true);
//    mutex.lock();
//    currentId = oldCurrentId;

//    requestQueue.remove(id);
//    mutex.unlock();
}

void QtCopyThread::handle(int id)
{
    if (cancelRequest)
        return;
    mutex.lock();
    QMap<int, Request>::ConstIterator it = requestQueue.find(id);
    Request r = it.value();
    int oldCurrentId = currentId;
    currentId = it.key();
    mutex.unlock();

    emit started(id);
    bool done = false;
    QtFileCopier::Error err = QtFileCopier::NoError;;
    while (!done) {
        mutex.lock();
        QMap<int, Request>::ConstIterator it = requestQueue.find(id);
        r = it.value();
        bool overwriteAll = overwriteAllRequest;
        mutex.unlock();
        CopyRequest copyRequest = r.request;

        ChainNode *n = 0;
        n = new CopyFileNode(n, id, copyRequest, this);
        n = new CopyDirNode(n);
        n = new MoveNode(n);
        n = new RenameNode(n);
        n = new FollowLinksNode(n);
        n = new MakeLinksNode(n);
        n = new OverwriteNode(n, r.overwrite || overwriteAll);
        n = new SourceExistsNode(n);
        n = new CanceledNode(n, r.canceled);

        done = n->handle();
        err = n->error();
        delete n;

        if (done || copyRequest.copyFlags & QtFileCopier::NonInteractive) {
            done = true;
            if (err != QtFileCopier::NoError)
                emit error(id, err, false);
        } else {
            mutex.lock();
            if (stopRequest || skipAllError.contains(err)) {
                done = true;
                if (!stopRequest)
                    emit error(id, err, false);
            } else {
                emit error(id, err, true);
                waitingForInteraction = true;
                interactionCondition.wait(&mutex);
                if (skipAllRequest) {
                    skipAllRequest = false;
                    skipAllError.insert(err);
                }
                waitingForInteraction = false;
            }
            mutex.unlock();
        }
    }

    emit finished(id, err != QtFileCopier::NoError);
    mutex.lock();
    currentId = oldCurrentId;
    requestQueue.remove(id);
    mutex.unlock();
}

void QtCopyThread::run()
{
    bool stop = false;

    while (!stop) {
        mutex.lock();
        if (requestQueue.isEmpty()) {
            if (stopRequest) {
                mutex.unlock();
                stop = true;
            } else {
                progressRequest = 0;
                cancelRequest = false;
                newCopyCondition.wait(&mutex);
                if (autoReset) {
                    overwriteAllRequest = false;
                    skipAllError.clear();
                }
                mutex.unlock();
            }
        } else {
            if (cancelRequest) {
                requestQueue.clear();
                cancelRequest = false;
                emit canceled();
                mutex.unlock();
            } else {
                mutex.unlock();
                handle(requestQueue.constBegin().key());
            }
        }
    }
    deleteLater();
}

class QtFileCopierPrivate
{
    QtFileCopier *q_ptr;
    Q_DECLARE_PUBLIC(QtFileCopier)
public:
    QtFileCopierPrivate();

    void setState(QtFileCopier::State s);
    void copyError(int id, QtFileCopier::Error error, bool stopped);
    void copyStarted(int id);
    void copyFinished(int id, bool err);
    void copyCanceled();
    int copy(const QString &sourceFile, const QString &destinationPath,
            QtFileCopier::CopyFlags flags, bool move);
    QList<int> copyFiles(const QStringList &sourceFiles,
        const QString &destinationDir, QtFileCopier::CopyFlags flags, bool move);
    QList<int> copyDirectory(const QString &sourceDir,
        const QString &destinationDir, QtFileCopier::CopyFlags flags, bool move);
    QMap<int, CopyRequest> copyDirectoryContents(const QString &sourceDir,
            const QString &destinationDir, QtFileCopier::CopyFlags flags, bool move);

    void progressRequest();

    void removeChildren(int id);
    CopyRequest prepareRequest(bool checkPath, const QString &sourceFile,
            const QString &destinationPath, QtFileCopier::CopyFlags flags,
            bool move, bool dir) const;
    void startThread();

    QtCopyThread *copyThread;
    QTimer *progressTimer;
    QtFileCopier::State state;
    bool error;
    int idCounter;
    QStack<int> currentStack;
    QMap<int, CopyRequest> requests;
    bool autoReset;
};

QtFileCopierPrivate::QtFileCopierPrivate()
{
    idCounter = 0;
    state = QtFileCopier::Idle;
    error = false;
    autoReset = true;
}

void QtFileCopierPrivate::setState(QtFileCopier::State s)
{
    if (state == s)
        return;
    Q_Q(QtFileCopier);
    if (s == QtFileCopier::Busy)
    {
        progressTimer->start();
        if (state == QtFileCopier::Idle)
            error = false;
    }
    else
    {
        progressTimer->stop();
    }
    emit q->stateChanged(s);
    state = s;
}

void QtFileCopierPrivate::copyError(int id, QtFileCopier::Error error, bool stopped)
{
    Q_Q(QtFileCopier);
    if (stopped == true)
        setState(QtFileCopier::WaitingForInteraction);
    emit q->error(id, error, stopped);
}

void QtFileCopierPrivate::copyStarted(int id)
{
    Q_Q(QtFileCopier);
    setState(QtFileCopier::Busy);
    currentStack.push(id);
    emit q->started(id);
}

void QtFileCopierPrivate::copyFinished(int id, bool err)
{
    Q_Q(QtFileCopier);
    int pop = currentStack.pop();
    Q_ASSERT(pop == id);
    Q_UNUSED(pop);
    emit q->finished(id, err);
    if (err) {
        error = err;
        removeChildren(id);
    }
    requests.remove(id);
    if (requests.isEmpty()) {
        setState(QtFileCopier::Idle);
        emit q->done(error);
    }
}

void QtFileCopierPrivate::removeChildren(int id)
{
    if (requests.contains(id)) {
        CopyRequest r = requests[id];
        QList<int> children = r.childrenQueue;
        QListIterator<int> it(children);
        while (it.hasNext()) {
            int childId = it.next();
            removeChildren(childId);
            requests.remove(childId);
        }
    }
}

void QtFileCopierPrivate::copyCanceled()
{
    Q_Q(QtFileCopier);
    requests.clear();
    emit q->canceled();
    setState(QtFileCopier::Idle);
    emit q->done(false);
}

CopyRequest QtFileCopierPrivate::prepareRequest(bool checkPath, const QString &sourceFile,
        const QString &destinationPath, QtFileCopier::CopyFlags flags, bool move, bool dir) const
{
    QFileInfo fis(sourceFile);
    QFileInfo fid(destinationPath);
    fid.makeAbsolute();
    if (checkPath && fid.isDir()) {
        QDir destDir(fid.filePath());
        fid.setFile(destDir, fis.fileName());
    }
    CopyRequest r;
    r.source = fis.filePath();
    r.dest = fid.filePath();
#if defined(Q_OS_WIN32)
    if (fis.isSymLink() && flags & QtFileCopier::FollowLinks) {
	// replace source with link dest
	r.source = fis.readLink();
	// remove .lnk from dest
	if (fid.suffix() == "lnk") {
            r.dest.remove(r.dest.length() - 4, 4);
	}
    } else if ((fis.isSymLink() || flags & QtFileCopier::MakeLinks) && fid.suffix() != "lnk")
        r.dest += QLatin1String(".lnk");
#endif
    r.copyFlags = flags;
    r.move = move;
    r.dir = dir;

    return r;
}

void QtFileCopierPrivate::startThread()
{
    QTimer::singleShot(0, copyThread, SLOT(restart()));
    /*
    if (state == QtFileCopier::Idle) {
        setState(QtFileCopier::Busy);
        error = false;
    }
    */
}

int QtFileCopierPrivate::copy(const QString &sourceFile, const QString &destinationPath,
            QtFileCopier::CopyFlags flags, bool move)
{
    CopyRequest r = prepareRequest(true, sourceFile, destinationPath, flags, move, false);
    requests[idCounter] = r;
    copyThread->copy(idCounter, r);
    startThread();
    return idCounter++;
}

QList<int> QtFileCopierPrivate::copyFiles(const QStringList &sourceFiles,
        const QString &destinationDir, QtFileCopier::CopyFlags flags, bool move)
{
    QMap<int, CopyRequest> resultList;
    QFileInfo fid(destinationDir);
    if (fid.isDir()) {
        QStringListIterator it(sourceFiles);
        while (it.hasNext()) {
            QFileInfo fis(it.next());
            if (!fis.isDir()) {
                CopyRequest r = prepareRequest(true, fis.filePath(), destinationDir,
                                flags, move, false);
                requests[idCounter] = r;
                resultList[idCounter] = r;
                idCounter++;
            } else {
                QMap<int, CopyRequest> tmp = copyDirectoryContents(fis.filePath(),
                                    destinationDir+'/'+fis.fileName(), flags, move);
                foreach(int k, tmp.keys()) {
                    CopyRequest r = tmp.value(k);
                    requests[k] = r;
                    resultList[k] = r;
                }
            }
        }
    }
    if (resultList.isEmpty())
        return QList<int>();
    copyThread->copy(resultList);
    startThread();
    return resultList.keys();
}

QList<int> QtFileCopierPrivate::copyDirectory(const QString &sourceDir,
        const QString &destinationDir, QtFileCopier::CopyFlags flags, bool move)
{
    QMap<int, CopyRequest> resultList;
    QFileInfo fis(sourceDir);
    fis.makeAbsolute();
    QFileInfo fid(destinationDir);
    fid.makeAbsolute();
    if (fis.exists() && fis.isDir()) {
        if (fid.exists() && fid.isDir()) {
            QDir sourceDir(fis.filePath());
            QDir destDir(fid.filePath());
            fid.setFile(destDir, sourceDir.dirName());
        }
        resultList = copyDirectoryContents(fis.filePath(),
                            fid.filePath(), flags, move);
        //QFile::setPermissions(fid.filePath(), fis.permissions());
    }

    if (resultList.isEmpty())
        return QList<int>();

    QMap<int, CopyRequest>::ConstIterator it = resultList.constBegin();
    while (it != resultList.constEnd()) {
        requests[it.key()] = it.value();
        it++;
    }

    copyThread->copy(resultList);
    startThread();

    return resultList.keys();
}

QMap<int, CopyRequest> QtFileCopierPrivate::copyDirectoryContents(const QString &sourceDir,
            const QString &destinationDir, QtFileCopier::CopyFlags flags, bool move)
{
    QMap<int, CopyRequest> resultList;
    QFileInfo fis(sourceDir);
    fis.makeAbsolute();
    QFileInfo fid(destinationDir);
    fid.makeAbsolute();

    CopyRequest r = prepareRequest(false, fis.filePath(), destinationDir, flags, move, true);
    resultList[idCounter] = r;
    int curId = idCounter;
    idCounter++;

    if (fis.isSymLink() && !(flags & QtFileCopier::FollowLinks))
        return resultList;

    if (flags & QtFileCopier::MakeLinks)
        return resultList;

    fis.setFile(r.source);
    fid.setFile(r.dest);
    QDir sDir(fis.filePath());
    QDir destDir(fid.filePath());
    QFileInfoList dirList = sDir.entryInfoList(QDir::Dirs);
    QListIterator<QFileInfo> itDir(dirList);
    while (itDir.hasNext()) {
        QFileInfo newfis = itDir.next();
        newfis.makeAbsolute();
        QString dirName = newfis.fileName();
        if (newfis.isDir() && dirName != QString(".") && dirName != QString("..")) {
            QFileInfo newfid(destDir.filePath(dirName));
            QMap<int, CopyRequest> childDir = copyDirectoryContents(newfis.filePath(),
                            newfid.filePath(), flags, move);
            resultList.unite(childDir);
            resultList[curId].childrenQueue.enqueue(childDir.constBegin().key());
        }
    }
    QFileInfoList fileList = sDir.entryInfoList(QDir::Files |
                    QDir::Hidden | QDir::System);
    QListIterator<QFileInfo> itLink(fileList);
    while (itLink.hasNext()) {
        QFileInfo newfis = itLink.next();
        if (!newfis.isDir() && newfis.isSymLink()) {
            newfis.makeAbsolute();
            CopyRequest r = prepareRequest(false, newfis.filePath(),
                        destDir.filePath(newfis.fileName()), flags, move, false);
            resultList[curId].childrenQueue.enqueue(idCounter);
            resultList[idCounter] = r;
            idCounter++;
        }
    }
    QListIterator<QFileInfo> itFile(fileList);
    while (itFile.hasNext()) {
        QFileInfo newfis = itFile.next();
        if (!newfis.isDir() && !newfis.isSymLink()) {
            newfis.makeAbsolute();
            CopyRequest r = prepareRequest(false, newfis.filePath(),
                        destDir.filePath(newfis.fileName()), flags, move, false);
            resultList[curId].childrenQueue.enqueue(idCounter);
            resultList[idCounter] = r;
            idCounter++;
        }
    }

    return resultList;
}

void QtFileCopierPrivate::progressRequest()
{
    if (state == QtFileCopier::Busy)
        copyThread->progress();
}

/*!
    \class QtFileCopier

    \brief The QtFileCopier class allows you to copy and move files in
    a background process.

    QtFileCopier provides a collection of functions performing the
    copy and move operations:

    \list
    \o copy()
    \o copyFiles()
    \o copyDirectory()
    \o move()
    \o moveFiles()
    \o moveDirectory()
    \endlist

    If you want to present visual feedback of an operation's progress
    in an console application or in a GUI application, you can either
    create an instance of the QtCopyDialog class, and connect it to
    your instance of the QtFileCopier class using the
    QtCopyDialog::setFileCopier() function, or you can let the
    application provide the visual feedback itself.

    The QtFileCopier class also provides a series of other functions,
    signals and slots.

    Operations are queued and performed one by one. You are notified
    about the beginning and end of each operation by the started() and
    finished() signals, respectively. In particular, when an operation
    concerns a directory, you are notified about the beginning and
    end of each sub-directory operation recursively.

    While performing the operations you are notified about the
    progress with the dataTransferProgress() signal. If an error
    occurs, the error() signal is emitted.

    You can retrieve information about current and pending operations
    using the pendingRequests() and currentId() functions. To retrieve
    information about a specific operation you can use the
    sourceFilePath(), destinationFilePath(), entryList() and isDir()
    functions, passing the operation's identifier as argument.

    All pending operations can be canceled with the cancelAll() slot,
    you can cancel any given operation by specifying its identifier as
    argument.

    By default, a file copier works in an interactive mode which means
    that when an error occurs, the operation is halted, the error()
    signal is emitted, and the file copier waits for the user's
    respons.  The user can choose to resume by skipping the operation,
    overwrite the destination or repeat the operation using the
    skip(), skipAll(), overwrite(), overwriteAll() or retry() slots.

    You can manipulate the behavior, and the result, of each operation
    with the QtFileCopier::CopyFlags passed with the copy() and move()
    functions. Note that when the QtFileCopier::NonInteractive flag is
    passed as argument, the operation will not halt if an error
    occurs.

    An instance of the QtFileCopier class is always in one of the
    states specified by the QtFileCopier::State enum. You are notified
    about transitions from one state to another by the stateChanged()
    signal. The new state is passed as argument.

    When all pending operations are completed, the done() signal is
    emitted.

    If you want to provide feedback and error handling, you can use
    your class constructor to create the file copier, and an
    associated copy dialog, and to connect the various QtFileCopier
    signals to your own custum slots. For example:

    \code
    QtFileCopier *copier;
    QtCopyDialog *copyDialog;

    MyClass::MyClass(QObject *parent)
    {
        copier = new QtFileCopier(parent);
        copyDialog = new QtCopyDialog(parent);
        copyDialog->setFileCopier(copier);

        QObject::connect(copier, SIGNAL(stateChanged(QtFileCopier::State)),
                         this, SLOT(stateChanged(QtFileCopier::State)));

        QObject::connect(copier, SIGNAL(done(bool)), this, SLOT(done(bool)));

        QObject::connect(copier, SIGNAL(error(int, QtFileCopier::Error, bool)),
                         this, SLOT(error(int, QtFileCopier::Error, bool)));
    }
    \endcode

    The copy dialog provides visual feedback, and the option of
    aborting the operation. The error handling can be implemented in a
    custom slot. In addition, you can implement any other respons to
    the file copier's signals:

    \code
    QMainWindow *mainWindow;

    MyClass::error(int identifier, QtFileCopier::Error error, bool stopped)
    {
        if (stopped == true) {
            ... // make interactive query
        }
    }

    MyClass::stateChanged(QtFileCopier::State state)
    {
        if (state != Idle) {
            mainWindow->setCursor(Qt::WaitCursor);
            ... // copying started
        } else {
            mainWindow->unsetCursor();
            ...
        }
    }

    MyClass::done(bool error)
    {
        mainWindow->statusBar()->showMessage(tr("Done"));
    }
    \endcode

    In the end, you can let your copy() function perform the required
    operations:

    \code
    QString sourceFile;
    QStringList sourceFiles;
    QString destinationPath;
    QString destinationDir;

    CopyFlags flags;

    MyClass::copy()
    {
        copier->copy(sourceFile, destinationPath, flags);
        copier->copyFiles(sourceFiles, destinationDir, flags);
        copier->move(sourceFile, destinationPath, flags);
    }
    \endcode

    If you choose to use the QtFileCopier class to perform the
    operations in a background process without error handling and visual
    feedback, you can implement the QtFileCopier construction in your
    copy() function:

    \code
    MyClass::copy()
    {
        QtFileCopier copier(this);
        QtFileCopier::QtCopyFlags flags = QtFileCopier::NonInteractive;
        copier.copy(sourceFile, destinationPath, flags);
        copier.copyFiles(sourceFiles, destinationDir, flags);
        copier.move(sourceFile, destinationPath, flags);
    }
    \endcode

    Note that when reaching the end of the MyClass::copy() function in
    the example above, the QtFileCopier destructor will not abort the
    operations currently performed, nor will it wait for the
    operations to be completed; the destructor will destroy the file
    copier while the operations will still be running in their own
    threads. When all tasks in a thread are done, the thread will
    automatically destroy itself.

    \sa QtCopyDialog

*/

/*!
    Creates a file copier with the given \a parent. The file copier's
    default state is QtFileCopier::Idle.
*/

QtFileCopier::QtFileCopier(QObject *parent) : QObject(parent)
{
    d_ptr = new QtFileCopierPrivate;
    d_ptr->q_ptr = this;
    d_ptr->copyThread = new QtCopyThread(this);
    d_ptr->progressTimer = new QTimer(this);
    d_ptr->progressTimer->setInterval(200);
    connect(d_ptr->progressTimer, SIGNAL(timeout()),
                this, SLOT(progressRequest()));
}

/*!
    Destroys the file copier.

    If there are pending operations, they will be still completed
    after the file copier's destruction. If the file copier was in the
    QtFileCopier::WaitingForInteraction state, the halted operation is
    resolved, but no operations are halted after the file copier is
    destroyed.

    To stop all the pending operations, you must call cancelAll() before
    destructing the file copier.
*/

QtFileCopier::~QtFileCopier()
{
    Q_D(QtFileCopier);
    delete d;
}

/*! \enum QtFileCopier::State

    This enum type defines the various states a file copier can
    have.

    An instance of the QtFileCopier class is always in one of the
    states specified by this enum. You are notified about transitions
    from one state to another by the stateChanged() signal. The
    default state is QtFileCopier::Idle.

    \value Idle The file copier is not performing any operation and has
           an empty pending list.
    \value Busy The file copier is performing an operation.
    \value WaitingForInteraction The file copier is waiting for the
           user's respons. To resume the operation use the skip(), skipAll(),
           overwrite(), overwriteAll() or retry() functions.

    \sa state(), stateChanged()
*/

/*! \enum QtFileCopier::CopyFlag
    This enum type is used to specify how QtFileCopier performs a copy or
    move operation.

    \value NonInteractive QtFileCopier ignores any errors that occur
           while performing the operation (i.e. an error in
           the operation  will not block the QtFileCopier's thread).
    \value Force When the destination file cannot be opened for writing,
           the QtFileCopier tries to remove it and open it again.
    \value MakeLinks The QtFileCopier makes symbolic links (or shortcuts
           on Windows) instead of copying.
    \value FollowLinks The QtFileCopier recursively follows symbolic
           links (or shortcuts on Windows) and copies the target instead
           of copying link itself.

    \sa copy(), move()
*/

/*! \enum QtFileCopier::Error

    This enum type is used to specify the various errors that can
    occur during a copy or move operation.

    \value NoError No error occured.
    \value SourceNotExists The source file doesn't exist.
    \value DestinationExists The destination file exists.
    \value SourceDirectoryOmitted The source was supposed to be a file, but
           the specified source is a directory (e.g. calling the copy()
           function).
    \value SourceFileOmitted The source was supposed to be a directory, but
           the specified source is a file (e.g. calling the copyDirectory()
           function).
    \value PathToDestinationNotExists The path to the destination
           file doesn't exist.
    \value CannotCreateDestinationDirectory The destination
           directory cannot be created.
    \value CannotOpenSourceFile The source file cannot be opened.
    \value CannotOpenDestinationFile The destination file cannot
           be opened.
    \value CannotRemoveDestinationFile The destination file cannot
           be removed (this error occurs when the QtFileCopier::Force flag
           is specified).
    \value CannotCreateSymLink The requested symbolic link (or shortcut
           on Windows) cannot be created.
    \value CannotReadSourceFile The source file cannot be read.
    \value CannotWriteDestinationFile The destination file cannot be written.
    \value CannotRemoveSource The source file cannot be removed (this error
           occurs when performing move operations).
    \value Canceled The operation was canceled.

    \sa error()
*/

/*!
    \fn void QtFileCopier::error(int identifier, QtFileCopier::Error error, bool stopped)

    This signal is emitted when an error occurs.

    The parameters are the \a identifier of the current operation, the
    type of \a error, and \a stopped which tells whether the operation
    is waiting for user respons or not. When an operation is halted
    due to an error (\a stopped == true), use the retry(), skip(),
    skipAll(), overwrite() or overwriteAll() slots to resume.

    \sa QtFileCopier::Error
*/

/*!
    \fn void QtFileCopier::stateChanged(QtFileCopier::State state)

    This signal is emitted when the state of the copier changes. The
    new \a state is passed as the signals argument.

    Note that the file copier's state changes \e after the signal is
    emitted.

    \sa state()
*/

/*!
    \fn void QtFileCopier::done(bool error)

    This signal is emitted when an operation is completed and there is
    no more pending operations. The \a error parameter is set to true
    if at least one operation failed, otherwise it is set to false.
*/

/*!
    \fn void QtFileCopier::started(int identifier)

    This signal is emitted when the operation with the given \a
    identifier starts.

    For each operation, two signals are emitted: the started() and
    finished() signals. Between those signals there can be emitted
    error() and dataTransferProgress() signals concerning the
    operation. When the operation concerns a directory, recursively
    generated started() and finished() signals can occur in between
    the inital signal pair as well.

    \sa finished()
*/

/*!
    \fn void QtFileCopier::dataTransferProgress(int identifier, qint64 progress)

    This signal is emitted approximately every 100 miliseconds to
    indicate the progress of the operation with the given \a
    identifier, It is only emitted when the file copier is in the
    QtFileCopier::Busy state. In addition the signal is emitted at the
    end of a successful operation, before the finished() signal is
    emitted.

    The \a progress parameter indicates how many bytes of the source
    file that are copied or moved.
*/

/*!
    \fn void QtFileCopier::finished(int identifier, bool error)

    This signal is emitted when the operation with the given \a
    identifier is completed. The \a error parameter is set to true if
    the operation failed, otherwise it is set to false.

    If the operation concerns a directory, and the error parameter is
    set to true, all the recursively generated operations will
    automatically be canceled.

    \sa started(), entryList()
*/

/*!
    \fn void QtFileCopier::canceled()

    This signal is emitted when the cancelAll() function is called, i.e.
    when all pending operations are canceled.

    \sa started(), pendingRequests()
*/

/*!
    Schedules an operation copying the \a sourceFile into the \a
    destinationPath, retaining the file name.

    The file copier will perform the copy operation according to the
    specified \a flags when the program execution returns to the event
    processing loop. The operation can only be scheduled when the file
    copier is in the QtFileCopier::Idle state.

    If the \a destinationPath points to an existing directory, the \a
    sourceFile is copied into that directory. If the \a
    destinationPath doesn't exist, the operation will fail.

    The function returns a non negative value as the operation
    identifier. If the operation is not valid, or the file copier is
    not in the appropiate state, the function returns -1.

    \sa copyFiles(), copyDirectory(), move()
*/

int QtFileCopier::copy(const QString &sourceFile, const QString &destinationPath,
                CopyFlags flags)
{
    if (state() != QtFileCopier::Idle)
        return -1;
    QFileInfo fis(sourceFile);
    if (fis.isDir())
        return -1; // Omitting Dir
    return d_ptr->copy(sourceFile, destinationPath, flags, false);
}

/*!
    Schedules operations copying the \a sourceFiles into the \a
    destinationDir, retaining the file names.

    The file copier will perform the copy operations according to the
    specified \a flags when the program execution returns to the event
    processing loop. The operations can only be scheduled when the
    file copier is in the QtFileCopier::Idle state.

    If a source in the \a sourceFiles list points to a directory then
    that source is omitted.  If the \a destinationDir doesn't exist,
    the function does nothing.

    The function returns a list of non negative values as the
    operation identifiers. The returned list can contain less items
    than the \a sourceFiles list in case some operations were not
    valid.

    \sa copy() copyDirectory() moveFiles()
*/

QList<int> QtFileCopier::copyFiles(const QStringList &sourceFiles,
                const QString &destinationDir, CopyFlags flags)
{
    if (state() != QtFileCopier::Idle)
        return QList<int>();
    return d_ptr->copyFiles(sourceFiles, destinationDir, flags, false);
}

/*!
    Schedules recursively operations copying the files in \a sourceDir
    into the \a destinationDir, retaining the file names.

    The file copier will perform the copy operations according to the
    specified \a flags when the program execution returns to the event
    processing loop. The operations can only be scheduled when the
    file copier is in the QtFileCopier::Idle state.

    If the \a destinationDir points to an existing directory, the \a
    sourceDir is copied into that directory. If the \a destinationDir
    doesn't exist, the operation will fail.

    The function returns a list of non negative values as the
    operation identifiers. The returned list can contain less items
    than the number of files in \a sourceDir in case some
    operations were not valid.

    \sa copy() copyFiles() moveDirectory()
*/

QList<int> QtFileCopier::copyDirectory(const QString &sourceDir,
                const QString &destinationDir, CopyFlags flags)
{
    if (state() != QtFileCopier::Idle)
        return QList<int>();
    return d_ptr->copyDirectory(sourceDir, destinationDir, flags, false);
}

/*!
    Schedules an operation moving the \a sourceFile into the \a
    destinationPath, retaining the file name.

    The file copier will perform the move operation according to the
    specified \a flags when the program execution returns to the event
    processing loop.  The operation can only be scheduled when the
    file copier is in the QtFileCopier::Idle state.

    \warning QtFileCopier doesn't accept, and ignores, the
    QtFileCopier::MakeLinks and QtFileCopier::FollowLinks for move
    operations.

    If the \a destinationPath points to an existing directory, the \a
    sourceFile is moved into that directory. If the \a destinationPath
    doesn't exist, the operation will fail.

    The function returns a non negative value as the operation
    identifier. If the operation is not valid, or the file copier is
    not in the appropiate state, the function returns -1.

    \sa moveFiles() moveDirectory() copy()
*/

int QtFileCopier::move(const QString &sourceFile, const QString &destinationPath,
                CopyFlags flags)
{
    if (flags & QtFileCopier::MakeLinks) {
        qWarning("QtFileCopier: cannot move with MakeLinks option specified, option cleared.");
        flags &= ~QtFileCopier::MakeLinks;
    }
    if (flags & QtFileCopier::FollowLinks) {
        qWarning("QtFileCopier: cannot move with FollowLinks option specified, option cleared.");
        flags &= ~QtFileCopier::FollowLinks;
    }
    QFileInfo fis(sourceFile);
    if (fis.isDir())
        return -1; // Omitting Dir
    return d_ptr->copy(sourceFile, destinationPath, flags, true);
}

/*!
    Schedules operations moving the \a sourceFiles into the \a
    destinationDir, retaining the file names.

    The file copier performs the move operations according to the
    specified \a flags when the program execution returns to the event
    processing loop. The operations can only be scheduled when the file
    copier is in the QtFileCopier::Idle state.

    \warning QtFileCopier doesn't accept, and ignores, the
    QtFileCopier::MakeLinks and QtFileCopier::FollowLinks for move
    operations.

    If a source in the \a sourceFiles list points to a directory then
    that source is omitted. If the \a destinationDir doesn't exist,
    the function does nothing.

    The function returns a list of non negative values as the
    operation identifiers. The returned list can contain less items
    than the \a sourceFiles list in case some operations were not
    valid.

    \sa move() moveDirectory() copyFiles()
*/

QList<int> QtFileCopier::moveFiles(const QStringList &sourceFiles,
                const QString &destinationDir, CopyFlags flags)
{
    if (flags & QtFileCopier::MakeLinks) {
        qWarning("QtFileCopier: cannot move with MakeLinks option specified, option cleared.");
        flags &= ~QtFileCopier::MakeLinks;
    }
    if (flags & QtFileCopier::FollowLinks) {
        qWarning("QtFileCopier: cannot move with FollowLinks option specified, option cleared.");
        flags &= ~QtFileCopier::FollowLinks;
    }
    return d_ptr->copyFiles(sourceFiles, destinationDir, flags, true);
}

/*!
    Schedules recursively operations moving the files in \a sourceDir
    into the \a destinationDir, retaining the file names.

    The file copier performs the move operations according to the
    specified \a flags when the program execution returns to the event
    processing loop. The operations can only be scheduled when the
    file copier is in the QtFileCopier::Idle state.

    \warning QtFileCopier doesn't accept, and ignores, the
    QtFileCopier::MakeLinks and QtFileCopier::FollowLinks for move
    operations.

    If the \a destinationDir points to an existing directory, the
    \a sourceDir is moved into that directory. If the destinationDir
    doesn't exist, the operation will fail.

    The function returns a list of non negative values as the
    operation identifiers. The returned list can contain less items
    than the number of files in \a sourceDir in case some operations
    were not valid.

    \sa move() moveFiles() copyDirectory()
*/

QList<int> QtFileCopier::moveDirectory(const QString &sourceDir,
                const QString &destinationDir, CopyFlags flags)
{
    if (flags & QtFileCopier::MakeLinks) {
        qWarning("QtFileCopier: cannot move with MakeLinks option specified, option cleared.");
        flags &= ~QtFileCopier::MakeLinks;
    }
    if (flags & QtFileCopier::FollowLinks) {
        qWarning("QtFileCopier: cannot move with FollowLinks option specified, option cleared.");
        flags &= ~QtFileCopier::FollowLinks;
    }
    return d_ptr->copyDirectory(sourceDir, destinationDir, flags, true);
}

/*!
    Returns a list of identifiers to the pending operation
    requests, including the currently performed operation.

    \sa sourceFilePath() destinationFilePath()
*/

QList<int> QtFileCopier::pendingRequests() const
{
    Q_D(const QtFileCopier);
    return d->requests.keys();
}

/*!
    \fn QString QtFileCopier::sourceFilePath(int identifier) const

    Returns the source path of the operation with the given \a
    identifier. If there is no matching pending operation, the
    function returns an empty string.

    \sa destinationFilePath()
*/

QString QtFileCopier::sourceFilePath(int id) const
{
    Q_D(const QtFileCopier);
    if (d->requests.contains(id))
        return d->requests[id].source;
    return QString();
}

/*!
    \fn QString QtFileCopier::destinationFilePath(int identifier) const

    Returns the destination path of the operation with the given \a
    identifier. If there is no matching pending operation, the
    function returns an empty string.

    \sa sourceFilePath()
*/

QString QtFileCopier::destinationFilePath(int id) const
{
    Q_D(const QtFileCopier);
    if (d->requests.contains(id))
        return d->requests[id].dest;
    return QString();
}

/*!
    \fn bool QtFileCopier::isDir(int identifier) const

    Returns whether the source of the operation with the given \a
    identifier is a directory. If there is no matching pending
    operation, the function returns false.

    \sa sourceFilePath(), entryList()
*/

bool QtFileCopier::isDir(int id) const
{
    Q_D(const QtFileCopier);
    if (d->requests.contains(id))
        return d->requests[id].dir;
    return false;
}

/*!
    \fn QList<int> QtFileCopier::entryList(int identifier) const

    Returns the list of recursively generated operations for the
    operation with the given \a identifier.  If the specified
    operation doesn't concern a directory, the function returns an
    empty list.

    \sa isDir()
*/

QList<int> QtFileCopier::entryList(int id) const
{
    Q_D(const QtFileCopier);
    if (d->requests.contains(id))
        return d->requests[id].childrenQueue;
    return QList<int>();
}

/*!
    Returns the currently performed operation's identifier, or -1 if
    there is no current operation (e.g. the file copier is in the
    QtFileCopier::Idle state).
*/

int QtFileCopier::currentId() const
{
    Q_D(const QtFileCopier);
    if (d->currentStack.isEmpty())
        return -1;
    return d->currentStack.top();

}

/*!
    Returns the current state of the file copier.
*/

QtFileCopier::State QtFileCopier::state() const
{
    Q_D(const QtFileCopier);
    return d->state;
}

/*!
    Cancels all operations, including the current one.
*/

void QtFileCopier::cancelAll()
{
    Q_D(QtFileCopier);
    d->copyThread->cancel();
}

/*!
    \fn void QtFileCopier::cancel(int identifier)

    Cancels the operation with the given \a identifier.
*/

void QtFileCopier::cancel(int id)
{
    Q_D(QtFileCopier);
    d->copyThread->cancel(id);
}

/*!
    Skips the current operation.

    If the current operation is applied to a directory, all
    recursively generated operations will also be skipped.

    The slot is used to resume when an operation has halted due to an
    error. For that reason it only applies while waiting for
    interaction (i.e the file copier is in the
    QtFileCopier::WaitingForInteraction state). In that case the file
    copier changes the state to QtFileCopier::Busy.

    \sa overwrite(), skipAll()
*/

void QtFileCopier::skip()
{
    if (state() != QtFileCopier::WaitingForInteraction)
        return;
    Q_D(QtFileCopier);
    d->copyThread->skip();
    d->setState(QtFileCopier::Busy);
}

/*!
    Skips all the subsequent operations resulting in the same error,
    including the current one.

    If an operation is applied to a directory, all recursively
    generated operations will also be skipped.

    The slot is used to resume when an operation has halted due to an
    error. For that reason it only applies while waiting for
    interaction (i.e the file copier is in the
    QtFileCopier::WaitingForInteraction state). In that case the file
    copier changes the state to QtFileCopier::Busy.

    \sa skip(), overwriteAll(), resetSkip()
*/

void QtFileCopier::skipAll()
{
    if (state() != QtFileCopier::WaitingForInteraction)
        return;
    Q_D(QtFileCopier);
    d->copyThread->skipAll();
    d->setState(QtFileCopier::Busy);
}

/*!
    Tries to perform the current (failing) operation once more.

    The slot only applies while waiting for interaction (i.e the file
    copier is in the QtFileCopier::WaitingForInteraction state). In
    that case the file copier changes the state to QtFileCopier::Busy.

    \sa skip(), overwrite()
*/

void QtFileCopier::retry()
{
    if (state() != QtFileCopier::WaitingForInteraction)
        return;
    Q_D(QtFileCopier);
    d->copyThread->retry();
    d->setState(QtFileCopier::Busy);
}

/*!
    Overwrites the current destination path. If the current
    destination is a directory, all subdirectory paths will be
    overwritten recursively.

    The slot is used to resume when an operation has halted due to an
    error. For that reason it only applies while waiting for
    interaction (i.e the file copier is in the
    QtFileCopier::WaitingForInteraction state). In that case, the file
    copier changes the state to QtFileCopier::Busy when this function
    is called.

    \sa skip(), overwriteAll()
*/

void QtFileCopier::overwrite()
{
    if (state() != QtFileCopier::WaitingForInteraction)
        return;
    Q_D(QtFileCopier);
    d->copyThread->overwrite();
    d->setState(QtFileCopier::Busy);
}

/*!
    Overwrites all the subsequent destination paths including the
    current one.

    If a destination path is a directory, all subdirectory paths will
    be overwritten recursively.

    The slot is used to resume when an operation has halted due to an
    error. For that reason it only applies while waiting for
    interaction (i.e the file copier is in the
    QtFileCopier::WaitingForInteraction state). In that case the file
    copier changes the state to QtFileCopier::Busy.

    \sa overwrite(), skipAll(), resetOverwrite()
*/

void QtFileCopier::overwriteAll()
{
    if (state() != QtFileCopier::WaitingForInteraction)
        return;
    Q_D(QtFileCopier);
    d->copyThread->overwriteAll();
    d->setState(QtFileCopier::Busy);
}

/*!
    Resets the file copier's default error handling behavior.

    \sa autoReset, resetSkip(), resetOverwrite()
*/

void QtFileCopier::reset()
{
    resetSkip();
    resetOverwrite();
}

/*!
    Resets the file copier's default behavior for error handling,
    i.e. entering the QtFileCopier::WaitingForInteraction state when
    \e any error occurs.

    \sa reset(), skipAll()
*/

void QtFileCopier::resetSkip()
{
    Q_D(QtFileCopier);
    d->copyThread->resetSkip();
}

/*!
    Resets the file copier's default behavior for handling
    QtFileCopier::DestinationExists errors, i.e. entering the
    QtFileCopier::WaitingForInteraction state.

    \sa reset(), overwriteAll()
*/

void QtFileCopier::resetOverwrite()
{
    Q_D(QtFileCopier);
    d->copyThread->resetOverwrite();
}

/*!
    \property QtFileCopier::autoReset

    \brief whether the file copier's default behavior is automatically
    reset when all pending operations are completed, or not.

    The default is true.

    Note that if this property is set to false, and for example, the
    last failing operation before the file copier entered the idle
    state was resolved by skipping all subsequent operations with the
    same error, the file copier will continue to skip operations with
    this error unless the reset() function is called explicitly.

    \sa reset()
*/

void QtFileCopier::setAutoReset(bool on)
{
    Q_D(QtFileCopier);
    d->autoReset = on;
    d->copyThread->setAutoReset(on);
}

bool QtFileCopier::autoReset() const
{
    Q_D(const QtFileCopier);
    return d->autoReset;
}

/*!
    \property QtFileCopier::progressInterval
    \brief the time that must pass before the next dataTransferProgress()
    signal is emitted.

    The default is 100 miliseconds.

    \sa dataTransferProgress()
*/

int QtFileCopier::progressInterval() const
{
    Q_D(const QtFileCopier);
    return d->progressTimer->interval();
}

void QtFileCopier::setProgressInterval(int ms)
{
    Q_D(QtFileCopier);
    d->progressTimer->setInterval(ms);
}

#include "qtfilecopier.moc"
#include "moc_qtfilecopier.cpp"