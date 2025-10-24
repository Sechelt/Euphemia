/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WGeometryWidget
#define H_WGeometryWidget

#include "W.h"

class WPosWidget;
class WSizeWidgetDouble;

class WGeometryWidget : public QWidget
{
    Q_OBJECT
public:
    WGeometryWidget( const QPointF &, const QSizeF &, QWidget *pParent, bool bToolBar = false );

    void setPos( const QPointF &pointPos );
    void setSize( const QSizeF &size );

    QPointF getPos();
    QSizeF  getSize();

signals:
    void signalChangedPos( const QPointF &pointPos );
    void signalChangedSize( const QSizeF &size );

public slots:
    void slotPos( const QPointF &pointPos );
    void slotSize( const QSizeF &size );

protected:
    WPosWidget *  pPosWidget;
    WSizeWidgetDouble * pSizeWidget;
};

#endif 

