/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef AWVALUELISTWIDGET_H
#define AWVALUELISTWIDGET_H

#include "W.h"

/*!
 * \class WValueListWidget 
 * \brief Edit a string list. 
 *  
 * This presents a list of the given strings. 
 * Allows the values to be; edited, added, and removed. 
 *  
 * \author pharvey (12/25/19)
 */
class WValueListWidget : public QMainWindow
{
    Q_OBJECT
public:
    WValueListWidget( const QStringList &listStrings, QWidget *pwidgetParent, const QString &stringName = QString() );
    virtual ~WValueListWidget();

    virtual QStringList getValues();

signals:
    void signalChanged( const QStringList & );

public slots:
    void slotRowChanged( int );
    void slotNew();
    void slotDelete();
    void slotUp();
    void slotDown();

protected:
    QAction *pActionNew;
    QAction *pActionDelete;
    QAction *pActionUp;
    QAction *pActionDown;

    QListWidget *pListWidget;
};

#endif 



