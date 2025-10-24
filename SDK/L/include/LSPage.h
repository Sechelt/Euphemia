/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LSPage
#define H_LSPage

#include <SSetting.h>

#include "L.h"

/*!
 * \brief Page/Canvas settings.
 * 
 * \author pharvey (4/27/23)
 */
class LSPage : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QSizeF sizeDefault READ getSize() WRITE setSize NOTIFY signalChanged)
    Q_PROPERTY(QSize sizeMargins READ getMargins WRITE setMargins NOTIFY signalChanged)
    Q_PROPERTY(QSize sizePages READ getPages WRITE setPages NOTIFY signalChanged)
    Q_PROPERTY(bool bPrinter READ getPrinter WRITE setPrinter NOTIFY signalChanged)
    Q_PROPERTY(bool bDrawMargins READ getDrawMargins WRITE setDrawMargins NOTIFY signalChanged)
    Q_PROPERTY(bool bAutoPages READ getAutoPages WRITE setAutoPages NOTIFY signalChanged)
public:
    LSPage();

    void setSize( const QSizeF & );
    void setMargins( const QSize & );
    void setPages( const QSize & );
    void setPrinter( bool );
    void setDrawMargins( bool );
    void setAutoPages( bool );
    void setSelected( const QSizeF &, const QSize &, const QSize &, bool, bool, bool );

    QSizeF  getSize()       { return sizeDefault;   }
    QSize   getMargins()    { return sizeMargins;   }
    QSize   getPages()      { return sizePages;     }
    bool    getPrinter()    { return bPrinter;      }
    bool    getDrawMargins(){ return bDrawMargins;  }
    bool    getAutoPages()  { return bAutoPages;    }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent );
    QWidget *getPanel( QWidget *pParent );

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem );
    void doLoad( QDomElement *pdomElem );

protected:
    QSizeF  sizeDefault     = QSizeF( 1000, 1000 );
    QSize   sizeMargins     = QSize( 5, 5 );
    QSize   sizePages       = QSize( 1, 1 );
    bool    bPrinter        = true;
    bool    bDrawMargins    = true;
    bool    bAutoPages      = true;
};

/*!
 * \brief   Widget to edit LSPage.
 *  
 * \author pharvey (3/14/23)
 */
class LSPageWidget : public QWidget
{
    Q_OBJECT
public:
    LSPageWidget( QWidget *pParent, Qt::Orientation n );

public slots:
    void slotRefresh();

protected:
    LSPage *    pSetting;
    QSpinBox  * pSizeX;
    QSpinBox  * pSizeY;
    QSpinBox  * pMarginsX;
    QSpinBox  * pMarginsY;
    QSpinBox  * pPagesX;
    QSpinBox  * pPagesY;
    QCheckBox * pPrinter;
    QCheckBox * pDrawMargins;
    QCheckBox * pAutoPages;

protected slots:
    void slotSizeX( int  );
    void slotSizeY( int  );
    void slotMarginsX( int  );
    void slotMarginsY( int  );
    void slotPagesX( int  );
    void slotPagesY( int  );
    void slotPrinter( bool  );
    void slotDrawMargins( bool  );
    void slotAutoPages( bool  );
};

#endif
