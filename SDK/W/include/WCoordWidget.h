/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WCoordWidget
#define H_WCoordWidget

#include "W.h"

class WCoordWidget : public QLabel
{
    Q_OBJECT
public:
    WCoordWidget( QWidget *pParent, const QString &stringLabel = QString() );

public slots:
    void slotCoord( int nCellX, int nCellY );
    void slotCoord( qreal nPixelX, qreal nPixelY );
    void slotCoord( const QPoint &pointCell );
    void slotCoord( const QPointF &pointPixel );

protected:
    QString stringLabel;
};

#endif 

