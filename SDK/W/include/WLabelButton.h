/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
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

