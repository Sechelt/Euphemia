/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 */

#ifndef H_SGrid
#define H_SGrid

#include "SSetting.h"

#include "S.h"

class SGrid : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(bool bVisible READ getVisible WRITE setVisible NOTIFY signalChanged)
    Q_PROPERTY(bool bRulerMajour READ getSizeRulerMajour WRITE setSizeRulerMajour NOTIFY signalChanged)
    Q_PROPERTY(bool bRulerMinor READ getSizeRulerMinor WRITE setSizeRulerMinor NOTIFY signalChanged)
    Q_PROPERTY(QSize sizeCell READ getSize WRITE setSize NOTIFY signalChanged)
    Q_PROPERTY(S::GridSnaps nSnap READ getSnap WRITE setSnap NOTIFY signalChanged)
public:
    SGrid( QObject *pParent = nullptr );

    void setVisible( bool b = true );
    void setSizeRulerMajour( bool b = true );
    void setSizeRulerMinor( bool b = true );
    void setSize( const QSize & );
    void setSnap( S::GridSnaps );
    void setSelected( bool, bool, bool, const QSize &, S::GridSnaps );

    bool            getVisible()        { return bVisible;      }
    bool            getSizeRulerMajour(){ return bRulerMajour;  }   
    bool            getSizeRulerMinor() { return bRulerMajour;  }   
    QSize           getSize()           { return sizeCell;      }
    S::GridSnaps    getSnap()           { return nSnap;         }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent );
    QWidget *getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    bool            bVisible    = false;            // draw grid
    bool            bRulerMajour= false;            // use ruler majour for size (has precedence over minor)
    bool            bRulerMinor = false;            // use ruler minor for size 
    QSize           sizeCell    = QSize( 48, 48 );  // use specific size (used if RulerMajour/RulerMinor not used)
    S::GridSnaps    nSnap       = S::GridSnapNone;
};

class SGridWidget : public QWidget
{
    Q_OBJECT
public:
    SGridWidget( QWidget *pParent, Qt::Orientation n, SGrid *pSetting = nullptr );

public slots:
    void slotRefresh();

protected:
    SGrid *     pSetting;
    QCheckBox * pVisible;
    QCheckBox * pRulerMajour;
    QCheckBox * pRulerMinor;
    QSpinBox *  pCellX;
    QSpinBox *  pCellY;
    QComboBox * pSnap;

protected slots:
    void slotVisible( bool );
    void slotRulerMajour( bool );
    void slotRulerMinor( bool );
    void slotCellX( int );
    void slotCellY( int );
    void slotSnap( int );
};

#endif
