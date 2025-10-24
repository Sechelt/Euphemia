/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WLabelButton.h"

WLabelButton::WLabelButton( QWidget *pParent )
    : QLabel( pParent )
{
}

void WLabelButton::mouseReleaseEvent( QMouseEvent *pEvent )
{
    QLabel::mouseReleaseEvent( pEvent );
    emit signalClicked();
}

