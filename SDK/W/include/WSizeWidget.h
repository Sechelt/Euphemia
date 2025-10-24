/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WSizeWidget
#define H_WSizeWidget

#include "W.h"

/*!
 * \brief Manage position.
 * 
 * \author pharvey (7/25/20)
 */
class WSizeWidgetDouble : public QWidget
{
    Q_OBJECT
public:
    WSizeWidgetDouble( const QSizeF &size, QWidget *pParent, bool bToolBar = false );

    void setValue( const QSizeF &size );

    QSizeF getValue();

signals:
    void signalChanged( const QSizeF &size );

public slots:
    void slotValue( const QSizeF &size );

protected:
    QDoubleSpinBox *pSpinBoxWidth;
    QDoubleSpinBox *pSpinBoxHeight;

protected slots:
    void slotChanged();
};

class WSizeWidgetInt : public QWidget
{
    Q_OBJECT
public:
    WSizeWidgetInt( const QSize &size, QWidget *pParent, bool bToolBar = false, bool bAspect = false );

    void setValue( const QSize &size );

    QSize getValue();

signals:
    void signalChanged( const QSize &size );

public slots:
    void slotValue( const QSize &size );

protected:
    QSize           sizeOriginal;
    QSpinBox *      pSpinBoxWidth;
    QSpinBox *      pSpinBoxHeight;
    QToolButton *   pButtonLockAspect;

protected slots:
    void slotChangedWidth();
    void slotChangedHeight();
    void slotLockAspect( bool );
};

#endif 

