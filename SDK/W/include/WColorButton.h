/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WColorButton
#define H_WColorButton

#include "W.h"

/*!
 * \class WColorButton 
 * \brief A button that can be used to select a color. 
 *  
 * The button can be used to set; pen, brush, or font color. 
 * This does not set other attributes of a; pen, brush or font... there are alternatives if needed.
 *  
 * Clicking this will invoke a \sa QColorDialog. 
 * The button will display the current color under an icon indicating type. 
 *  
 * \sa WPenButton 
 * \sa WTextButton 
 * \sa WBrushButton 
 * 
 * \author pharvey (4/19/22)
 */
class WColorButton : public QToolButton
{
    Q_OBJECT
public:
    enum Types
    {
        Font,
        Pen,
        Brush,
        Fill,
        Color
    };

    WColorButton( const QColor &color, QWidget *pParent, Types n );

    void setValue( const QColor &color );

    QColor getValue() { return color; }

signals:
    void signalChanged( const QColor &color );

protected:
    Types nType;
    QColor color;
    QPixmap getIcon( int nSize = 32 );

protected slots:
    void slotClicked();
};

#endif 

