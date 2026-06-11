/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WLabelButton
#define H_WLabelButton

#include "W.h"

class WLabelButton : public QLabel
{
    Q_OBJECT
public:
    explicit WLabelButton( QWidget *pParent );

signals:
    void signalClicked();

protected:
    void mouseReleaseEvent( QMouseEvent *pEvent );
};

#endif 

