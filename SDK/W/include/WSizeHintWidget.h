/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WSizeHintWidget
#define H_WSizeHintWidget

#include "W.h"

/*!
 * \brief Manage position.
 * 
 * \author pharvey (7/25/20)
 */
class WSizeHintWidget : public QWidget
{
    Q_OBJECT
public:
    WSizeHintWidget( const QSizeF &size, QWidget *pParent, bool bToolBar = false );

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

#endif 

