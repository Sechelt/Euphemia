/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WBitmapWidget
#define H_WBitmapWidget

#include "W.h"

class WBitmapButton;

/*!
 * \brief Allow selecting a bitmap and its placement. 
 *  
 * \sa W::WImagePlacements 
 * 
 * \author pharvey (7/29/20)
 */
class WBitmapWidget : public QWidget
{
    Q_OBJECT
public:
    WBitmapWidget( bool bUse, const QPixmap &pixmap, W::WImagePlacements nPlacement, QWidget *pParent );

signals:
    // to request change in object
    void signalUse( bool );
    void signalPlacement( W::WImagePlacements nPlacement );
    void signalBitmap( const QPixmap &pixmap );
    
public slots:
    // to register value in object (may be different than requested)
    void slotUse( bool );
    void slotPlacement( W::WImagePlacements nPlacement );
    void slotBitmap( const QPixmap &pixmap );

protected:
    QCheckBox *     pUseBitMap;
    QComboBox *     pPlacement;
    WBitmapButton * pBitmapButton;

protected slots:
    void slotChangedPlacement( int nIndex );
};

#endif


