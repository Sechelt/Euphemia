/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WFontColorButton
#define H_WFontColorButton

#include <CBD.h>

#include "W.h"

class WFontColorButton : public QToolButton
{
    Q_OBJECT
public:
    WFontColorButton( const QColor &colorForeground, const QBrush &brushBackground, QWidget *pParent );

    void setForeground( const QColor &color );
    void setBackground( const QBrush &brush );

    QColor getForeground() { return colorForeground; }
    QBrush getBackground() { return brushBackground; }

signals:
    void signalChanged( const QColor &color, const QBrush &bush );

protected:
    QColor  colorForeground;
    QBrush  brushBackground;
    QPixmap getIcon( int nSize = 32 );

protected slots:
    void slotClicked();
};

#endif 

