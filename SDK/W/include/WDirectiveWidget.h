/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef WDIRECTIVEWIDGET_H
#define WDIRECTIVEWIDGET_H

#include "W.h"

class WDirectiveWidget : public QWidget
{
    Q_OBJECT
public:
    WDirectiveWidget( const QIcon &icon, QTableWidget *pTableWidget, QWidget *pParent );
    ~WDirectiveWidget();
    
signals:
    void signalDo();
    void signalDo( int nRow );

protected:
    QAction *       pActionSelect;
    QPushButton *   pButtonDo;
    QToolButton *   pButtonSelect;
    QTableWidget *  pTableWidget;

protected slots:
    virtual void slotDo();
    virtual void slotSelect();
    virtual void slotItemClicked( QTableWidgetItem *pItem );
};

#endif 

