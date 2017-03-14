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

#ifndef QTCOPYDIALOG_H
#define QTCOPYDIALOG_H

#include <QDialog>
#include "qtfilecopier.h"

class QtCopyDialogPrivate;

class QT_QTCOPYDIALOG_EXPORT QtCopyDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int minimumDuration READ minimumDuration WRITE setMinimumDuration)
    Q_PROPERTY(bool autoClose READ autoClose WRITE setAutoClose)
public:

    QtCopyDialog(QWidget *parent = 0);
    QtCopyDialog(QtFileCopier *copier, QWidget *parent = 0);
    ~QtCopyDialog();

    void setFileCopier(QtFileCopier *copier);
    QtFileCopier *fileCopier() const;

    int minimumDuration() const;
    void setMinimumDuration(int ms);

    bool autoClose() const;
    void setAutoClose(bool b);

public Q_SLOTS:

    void reject();

private:

    QtCopyDialogPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtCopyDialog)
    Q_DISABLE_COPY(QtCopyDialog)

    Q_PRIVATE_SLOT(d_func(), void error(int, QtFileCopier::Error, bool))
    Q_PRIVATE_SLOT(d_func(), void stateChanged(QtFileCopier::State))
    Q_PRIVATE_SLOT(d_func(), void done(bool))
    Q_PRIVATE_SLOT(d_func(), void started(int))
    Q_PRIVATE_SLOT(d_func(), void dataTransferProgress(int, qint64))
    Q_PRIVATE_SLOT(d_func(), void finished(int, bool))
    Q_PRIVATE_SLOT(d_func(), void canceled())
    Q_PRIVATE_SLOT(d_func(), void showProgress())
    Q_PRIVATE_SLOT(d_func(), void showDialog())

};

#endif
