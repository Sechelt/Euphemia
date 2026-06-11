/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
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

