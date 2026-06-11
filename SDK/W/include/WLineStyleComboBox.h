/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WLineStyleComboBox
#define H_WLineStyleComboBox

#include "W.h"

#include "W.h"

class WLineStyleComboBox : public QComboBox
{
    Q_OBJECT
public:
    WLineStyleComboBox( Qt::PenStyle n, QWidget *pParent );

    void setValue( Qt::PenStyle n );
    Qt::PenStyle getValue();

protected:
    QPixmap getIcon( Qt::PenStyle n, int width = 64, int height = 16 );

signals:
    void signalChanged( Qt::PenStyle n );

private slots:
    void slotChanged( int nIndex );
};

#endif 

