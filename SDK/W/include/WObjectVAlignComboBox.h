/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WObjectVAlignComboBox
#define H_WObjectVAlignComboBox

#include "W.h"

#include "W.h"

class WObjectVAlignComboBox : public QComboBox
{
    Q_OBJECT
public:
    WObjectVAlignComboBox( Qt::AlignmentFlag n, QWidget *pParent, bool bText = false );

    void setValue( Qt::AlignmentFlag n );
    Qt::AlignmentFlag getValue();

signals:
    void signalChanged( Qt::AlignmentFlag n );

private slots:
    void slotChanged( int nIndex );
};

#endif 

