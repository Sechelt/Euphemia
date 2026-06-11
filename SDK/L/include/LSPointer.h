/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_LSPointer
#define H_LSPointer

#include <SSetting.h>
#include <WImageButton.h>
#include <WPen.h>

#include "L.h"

/*!
 * \brief Mouse pointer settings.
 * 
 * \author pharvey (4/29/23)
 */
class LSPointer : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QCursor cursor READ getCursor)
    Q_PROPERTY(QSize sizeCursor READ getSizeCursor)
    Q_PROPERTY(Qt::CursorShape nStandard READ getStandard WRITE setStandard NOTIFY signalChanged)
    Q_PROPERTY(QString stringResource READ getResource WRITE setResource NOTIFY signalChanged)
    Q_PROPERTY(QImage image READ getImage WRITE setImage NOTIFY signalChanged)
    Q_PROPERTY(QPoint pointHotSpot READ getHotSpot WRITE setHotSpot NOTIFY signalChanged)
    Q_PROPERTY(bool bCrossHairs READ getCrossHairs WRITE setCrossHairs NOTIFY signalChanged)
    Q_PROPERTY(QPen pen READ getPen WRITE setPen NOTIFY signalChanged)
public:
    LSPointer();

    void setStandard( Qt::CursorShape );
    void setResource( const QString & );
    void setImage( const QImage & );
    void setHotSpot( const QPoint & );
    void setCrossHairs( bool );
    void setPen( const QPen & );
    void setSelected( Qt::CursorShape, const QString &, const QImage &, const QPoint &, bool, const QPen & );

    QCursor         getCursor();
    QSize           getSizeCursor();
    Qt::CursorShape getStandard()   { return nStandard;         }
    QString         getResource()   { return stringResource;    }
    QImage          getImage()      { return image;             }
    QPoint          getHotSpot()    { return pointHotSpot;      }
    bool            getCrossHairs() { return bCrossHairs;       }
    QPen            getPen()        { return pen;               }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent );
    QWidget *getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    QCursor         cursor;     // ignore value - needed but never set
    QSize           sizeCursor; // ignore value - needed but never set
    Qt::CursorShape nStandard   = Qt::ArrowCursor;
    QString         stringResource;
    QImage          image;
    QPoint          pointHotSpot= QPoint( 0, 0 );    
    bool            bCrossHairs = false;
    QPen            pen;    
};

/*!
 * \brief   Widget to edit LSPointer.
 *  
 * \author pharvey (3/14/23)
 */
class LSPointerWidget : public QWidget
{
    Q_OBJECT
public:
    LSPointerWidget( QWidget *pParent, Qt::Orientation n );

protected:
    LSPointer *   pSetting;
    QComboBox *   pStandard;       
    QComboBox *   pResource;       
    WImageButton *pImage;
    QSpinBox  *   pHotSpotX;
    QSpinBox  *   pHotSpotY;
    QCheckBox *   pCrossHairs;
    WPenToolBar *  pPen;

protected slots:
    void slotRefresh();
    void slotStandard( int );
    void slotResource( int );
    void slotImage();
    void slotHotSpotX( int  );
    void slotHotSpotY( int  );
    void slotCrossHairs( bool );
    void slotPen( const QPen & );
};

#endif
