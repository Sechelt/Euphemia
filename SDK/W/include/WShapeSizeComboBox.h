/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WShapeSizeComboBox
#define H_WShapeSizeComboBox

#include "W.h"

#include "W.h"

class WShapeSizeComboBox : public QComboBox
{
    Q_OBJECT
public:
    WShapeSizeComboBox( QWidget *pParent );

    void setValue( int n );
    int getValue();

signals:
    void signalChanged( int n );

public slots:
    void slotValue( int n );

private slots:
    void slotChanged( int nIndex );
};

#endif 

