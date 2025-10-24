/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
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

