/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
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

