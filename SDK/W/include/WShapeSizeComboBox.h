/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
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

