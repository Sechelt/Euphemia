/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WSizeDialog
#define H_WSizeDialog

#include "W.h"

class WSizeWidgetInt;

class WSizeDialogInt : public QDialog
{
    Q_OBJECT
public:
    WSizeDialogInt( const QSize &size, QWidget *pParent = nullptr, bool bAspect = false );

    QSize getSize();
    static QSize getSize( bool *pOk, const QSize &size, QWidget *pParent, bool bAspect = false );

protected:
    WSizeWidgetInt *pSizeWidget;
};

#endif 

