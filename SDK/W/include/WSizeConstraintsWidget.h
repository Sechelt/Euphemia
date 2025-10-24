/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WSizeConstraintsWidget
#define H_WSizeConstraintsWidget

#include "W.h"

/*!
 * \brief Manage mininum and maximum size.
 *  
 * Here we manage the; 
 *  
 * - minimum and maximum Width
 * - minimum and maximum Height
 *  
 * The values entered are *requested* via a signal sent to the object.
 * The actual values (if changed) come in via signal from the object. 
 *  
 * The interface is; 
 *  
 * - a pair of spin boxs for Width 
 * - a pair of spin boxs for Height 
 *  
 * \param  sizeMinumum - min Width,Height 
 * \param  sizeMaximum - max Width,Height
 *  
 * \author pharvey (7/25/20)
 */
class WSizeConstraintsWidget : public QWidget
{
    Q_OBJECT
public:
    WSizeConstraintsWidget( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum, QWidget *pParent, bool bToolBar = false );

    void setValue( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum );
    void setMinimum( const QSizeF &size );
    void setMaximum( const QSizeF &size );

    QSizeF getMinimum() { return QSize( pSpinBoxMinWidth->value(), pSpinBoxMinHeight->value() ); }
    QSizeF getMaximum() { return QSize( pSpinBoxMaxWidth->value(), pSpinBoxMaxHeight->value() ); }

signals:
    void signalChanged( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum );

public slots:
    void slotValue( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum );

protected:
    QDoubleSpinBox *pSpinBoxMinWidth;
    QDoubleSpinBox *pSpinBoxMinHeight;

    QDoubleSpinBox *pSpinBoxMaxWidth;
    QDoubleSpinBox *pSpinBoxMaxHeight;

protected slots:
    void slotChanged();
};

#endif 

