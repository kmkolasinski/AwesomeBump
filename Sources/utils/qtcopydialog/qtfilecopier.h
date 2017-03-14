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

#ifndef QTFILECOPIER_H
#define QTFILECOPIER_H

#include <QtCore/QObject>

#if defined(Q_WS_WIN)
#  if !defined(QT_QTCOPYDIALOG_EXPORT) && !defined(QT_QTCOPYDIALOG_IMPORT)
#    define QT_QTCOPYDIALOG_EXPORT
#  elif defined(QT_QTCOPYDIALOG_IMPORT)
#    if defined(QT_QTCOPYDIALOG_EXPORT)
#      undef QT_QTCOPYDIALOG_EXPORT
#    endif
#    define QT_QTCOPYDIALOG_EXPORT __declspec(dllimport)
#  elif defined(QT_QTCOPYDIALOG_EXPORT)
#    undef QT_QTCOPYDIALOG_EXPORT
#    define QT_QTCOPYDIALOG_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTCOPYDIALOG_EXPORT
#endif

class QtFileCopierPrivate;

class QT_QTCOPYDIALOG_EXPORT QtFileCopier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int progressInterval READ progressInterval WRITE setProgressInterval)
    Q_PROPERTY(bool autoReset READ autoReset WRITE setAutoReset)
public:

    QtFileCopier(QObject *parent = 0);
    ~QtFileCopier();

    enum State {
        Idle,
        Busy,
        WaitingForInteraction
    };

    enum CopyFlag {
        NonInteractive = 0x01,
        Force = 0x02,
        MakeLinks = 0x04,
        FollowLinks = 0x08 // if not set links are copied
    };

    enum Error {
        NoError,
        SourceNotExists,
        DestinationExists,
        SourceDirectoryOmitted,
        SourceFileOmitted,
        PathToDestinationNotExists,
        CannotCreateDestinationDirectory,
        CannotOpenSourceFile,
        CannotOpenDestinationFile,
        CannotRemoveDestinationFile,
        CannotCreateSymLink,
        CannotReadSourceFile,
        CannotWriteDestinationFile,
        CannotRemoveSource,
        Canceled
    };

    Q_DECLARE_FLAGS(CopyFlags, CopyFlag)

    int copy(const QString &sourceFile, const QString &destinationPath,
                CopyFlags flags = 0);
    QList<int> copyFiles(const QStringList &sourceFiles, const QString &destinationDir,
                CopyFlags flags = 0);
    QList<int> copyDirectory(const QString &sourceDir, const QString &destinationDir,
                CopyFlags flags = 0);

    int move(const QString &sourceFile, const QString &destinationPath,
                CopyFlags flags = 0);
    QList<int> moveFiles(const QStringList &sourceFiles, const QString &destinationDir,
                CopyFlags flags = 0);
    QList<int> moveDirectory(const QString &sourceDir, const QString &destinationDir,
                CopyFlags flags = 0);

    QList<int> pendingRequests() const;
    QString sourceFilePath(int id) const;
    QString destinationFilePath(int id) const;
    bool isDir(int id) const;
    QList<int> entryList(int id) const;
    int currentId() const;

    State state() const;

    void setAutoReset(bool on);
    bool autoReset() const;
    int progressInterval() const;
    void setProgressInterval(int ms);

public Q_SLOTS:

    void cancelAll();
    void cancel(int id);

    void skip();
    void skipAll();
    void retry();

    void overwrite();
    void overwriteAll();

    void reset();
    void resetSkip();
    void resetOverwrite();

Q_SIGNALS:
    void error(int id, QtFileCopier::Error error, bool stopped);

    void stateChanged(QtFileCopier::State state);

    void done(bool error);
    void started(int id);
    void dataTransferProgress(int id, qint64 progress);
    void finished(int id, bool error);
    void canceled();

private:

    QtFileCopierPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtFileCopier)
    Q_DISABLE_COPY(QtFileCopier)

    Q_PRIVATE_SLOT(d_func(), void copyStarted(int))
    Q_PRIVATE_SLOT(d_func(), void copyFinished(int, bool))
    Q_PRIVATE_SLOT(d_func(), void copyCanceled())
    Q_PRIVATE_SLOT(d_func(), void copyError(int, QtFileCopier::Error, bool))
    Q_PRIVATE_SLOT(d_func(), void progressRequest())
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QtFileCopier::CopyFlags)

#endif
