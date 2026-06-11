/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WMarginsWidget
#define H_WMarginsWidget

#include "W.h"

class WMarginsWidget : public QWidget
{
    Q_OBJECT
public:
    WMarginsWidget( const QMargins &m, QWidget *pParent, bool bToolBar = false );

    void setValue( const QMargins &m );

    QMargins getValue();

signals:
    void signalChanged( const QMargins &m );

public slots:
    void slotValue( const QMargins &m );

protected:
    QSpinBox *pSpinBoxLeft;
    QSpinBox *pSpinBoxRight;
    QSpinBox *pSpinBoxTop;
    QSpinBox *pSpinBoxBottom;

protected slots:
    void slotChanged();
};

#endif 

