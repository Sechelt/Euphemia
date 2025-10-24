/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LSGrid
#define H_LSGrid

#include <SSetting.h>

#include "L.h"

/*!
 * \brief Preferences/Context settings.
 * 
 * \author pharvey (3/15/23)
 */
class LSGrid : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(L::GridTypes nType READ getType WRITE setType NOTIFY signalChanged)
    Q_PROPERTY(L::GridSnaps nSnap READ getSnap WRITE setSnap NOTIFY signalChanged)
    Q_PROPERTY(QSize sizeCell READ getCellSize WRITE setCellSize NOTIFY signalChanged)
    Q_PROPERTY(int nSizeMin READ getSizeMin WRITE setSizeMin NOTIFY signalChanged)
public:
    LSGrid();

    void setType( L::GridTypes );
    void setSnap( L::GridSnaps );
    void setCellSize( const QSize & );
    void setSizeMin( int );
    void setSelected( L::GridTypes, L::GridSnaps, const QSize &, int );

    L::GridTypes    getType()       { return nType;     }
    L::GridSnaps    getSnap()       { return nSnap;     }
    QSize           getCellSize()   { return sizeCell;  }
    int             getSizeMin()    { return nSizeMin;  }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent );
    QWidget *getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    L::GridTypes    nType       = L::GridTypeStandard;
    L::GridSnaps    nSnap       = L::GridSnapNone;
    QSize           sizeCell    = QSize( 64, 64 );      /*!< GridTypeStandard: cell width (never let it be < 1)         */
    int             nSizeMin    = 4;                    /*!< GridTypePixel: size to start drawing grid                  */
};

/*!
 * \brief   Widget to edit LSGrid.
 *  
 * \author pharvey (3/14/23)
 */
class LSGridWidget : public QWidget
{
    Q_OBJECT
public:
    LSGridWidget( QWidget *pParent, Qt::Orientation n );

public slots:
    void slotRefresh();

protected:
    LSGrid *    pSetting;
    QComboBox * pType;
    QComboBox * pSnap;
    QSpinBox *  pCellX;
    QSpinBox *  pCellY;
    QSpinBox *  pMin;

protected slots:
    void slotType( int );
    void slotSnap( int );
    void slotCellX( int );
    void slotCellY( int );
    void slotMin( int );
};

#endif
