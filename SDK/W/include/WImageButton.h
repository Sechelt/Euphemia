/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WImageButton
#define H_WImageButton

#include "W.h"

class WImageButton : public QWidget
{
    Q_OBJECT
public:
    WImageButton( QWidget *pParent, const QImage &image );

    void setImage( const QImage &image );
    QImage getImage() { return image; }

signals:
    void signalClick();

protected:
    QImage image;

    void paintEvent( QPaintEvent *pEvent );
    void mouseDoubleClickEvent( QMouseEvent *pEvent );
};

#endif 

