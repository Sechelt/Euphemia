/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_WFindTextDialog
#define H_WFindTextDialog

#include "W.h"

class WFindTextWidget;

class WFindTextDialog : public QDialog
{
    Q_OBJECT
public:
    WFindTextDialog( QWidget *pParent = nullptr );
    ~WFindTextDialog();

    void doClear();

    bool isFinding() { return bFinding; }

signals:
    void signalFindFirst( const QString &, bool, bool, bool );
    void signalFindNext();
    void signalFindPrevious();
    void signalReplace( const QString & );
    void signalReplaceAll( const QString & );
    void signalFindTextChanged();

protected:
    WFindTextWidget *pFindTextWidget;
    bool bFinding = false;

    void doLoadState();
    void doSaveState();

protected slots:
    void slotFindNext();
    void slotFindPrevious();
    void slotReplace();
    void slotReplaceAll();
    void slotFindTextChanged( const QString & );
};

#endif 

