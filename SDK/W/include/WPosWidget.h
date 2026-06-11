/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WPosWidget
#define H_WPosWidget

#include "W.h"

/*!
 * \brief Manage position.
 * 
 * \author pharvey (7/25/20)
 */
class WPosWidget : public QWidget
{
    Q_OBJECT
public:
    WPosWidget( const QPointF &pointPos, QWidget *pParent, bool bToolBar = false );

    void setValue( const QPointF &pointPos );

    QPointF getValue();

signals:
    void signalChanged( const QPointF &pointPos );

public slots:
    void slotValue( const QPointF &pointPos );

protected:
    QDoubleSpinBox *pSpinBoxX;
    QDoubleSpinBox *pSpinBoxY;

protected slots:
    void slotChanged();
};

#endif 

