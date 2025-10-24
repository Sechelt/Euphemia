/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WLayoutWidget
#define H_WLayoutWidget

#include "W.h"

class WSizeHintWidget;
class WMarginsWidget;
class WStretchWidget;
class WSizeConstraintsWidget;

class WLayoutWidget : public QWidget
{
    Q_OBJECT
public:
    WLayoutWidget( QWidget *pParent, bool bToolBar = false );

    void setSizeHint( const QSizeF &size );
    void setMargins( const QMargins &m );
    void setSizeConstraints( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum );
    void setStretch( const QSizeF &sizeStretch );

    QSizeF      getSizeHint();
    QMargins    getMargins();
    QSizeF      getSizeConstraintMinimum();
    QSizeF      getSizeConstraintMaximum();
    QSizeF      getStretch();

signals:
    void signalChangedSizeHint( const QSizeF &size );
    void signalChangedMargins( const QMargins &m );
    void signalChangedSizeConstraints( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum );
    void signalChangedStretch( const QSizeF &sizeStretch );

public slots:
    void slotSizeHint( const QSizeF &size );
    void slotMargins( const QMargins &m );
    void slotSizeConstraints( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum );
    void slotStretch( const QSizeF &size );

protected:
    WSizeHintWidget *       pSizeHintWidget;
    WMarginsWidget *        pMarginsWidget;
    WSizeConstraintsWidget *pSizeConstraintsWidget;
    WStretchWidget *        pStretchWidget;
};

#endif 

