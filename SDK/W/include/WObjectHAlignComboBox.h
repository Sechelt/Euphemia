/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WObjectHAlignComboBox
#define H_WObjectHAlignComboBox

#include "W.h"

class WObjectHAlignComboBox : public QComboBox
{
    Q_OBJECT
public:
    WObjectHAlignComboBox( Qt::AlignmentFlag n, QWidget *pParent, bool bText = false );

    void setValue( Qt::AlignmentFlag n );
    Qt::AlignmentFlag getValue();

signals:
    void signalChanged( Qt::AlignmentFlag n );

private slots:
    void slotChanged( int nIndex );
};

#endif 

