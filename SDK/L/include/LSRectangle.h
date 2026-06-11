/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LSRectangle
#define H_LSRectangle

#include "SSetting.h"

/*!
 * \brief Preferences/Context settings for LSRectangle.
 * 
 * \author pharvey (3/15/23)
 */
class LSRectangle : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(bool bBind READ getBind WRITE setBind NOTIFY signalChanged)
    Q_PROPERTY(Qt::SizeMode nSizeMode READ getSizeMode WRITE setSizeMode NOTIFY signalChanged)
    Q_PROPERTY(qreal nXRadius READ getXRadius WRITE setXRadius NOTIFY signalChanged)
    Q_PROPERTY(qreal nYRadius READ getYRadius WRITE setYRadius NOTIFY signalChanged)
public:
    LSRectangle();

    void setBind( bool );
    void setSizeMode( Qt::SizeMode );     
    void setXRadius( qreal );      
    void setYRadius( qreal );      
    void setSelected( bool, Qt::SizeMode, qreal, qreal );

    bool            getBind()       { return bBind;     }
    Qt::SizeMode    getSizeMode()   { return nSizeMode; }   
    qreal           getXRadius()    { return nXRadius;  }   
    qreal           getYRadius()    { return nYRadius;  }   

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent ) override;
    QWidget *getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    bool            bBind           = false;            /*!< true = make squares/circles                                */
    Qt::SizeMode    nSizeMode       = Qt::RelativeSize; /*!< for rounded rect                                           */
    qreal           nXRadius        = 0;                /*!< for rounded rect                                           */
    qreal           nYRadius        = 0;                /*!< for rounded rect                                           */
};

/*!
 * \brief   Edit widget for use in a tool bar. 
 *          Edits LSRectangle.
 *  
 * Horizontal layout.
 * 
 * \author pharvey (3/14/23)
 */
class LSRectangleToolBar : public QWidget
{
    Q_OBJECT
public:
    LSRectangleToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotBind( Qt::CheckState );
    void slotMode( int );
    void slotXRadius( qreal );
    void slotYRadius( qreal );

protected:
    LSRectangle *    pSetting;

    QCheckBox *     pBind;
    QComboBox *     pMode       = nullptr;
    QDoubleSpinBox *pXRadius    = nullptr;
    QDoubleSpinBox *pYRadius    = nullptr;
};

/*!
 * \brief   Edit widget for use in a dialog or a dock.
 *          Edits LSRectangle.
 *  
 * Form (Vertical) layout with labels.
 * 
 * \author pharvey (3/14/23)
 */
class LSRectanglePanel : public QWidget
{
    Q_OBJECT
public:
    LSRectanglePanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotBind( Qt::CheckState );
    void slotMode( int );
    void slotXRadius( qreal );
    void slotYRadius( qreal );

protected:
    LSRectangle *    pSetting;

    QCheckBox *     pBind;
    QComboBox *     pMode       = nullptr;
    QDoubleSpinBox *pXRadius    = nullptr;
    QDoubleSpinBox *pYRadius    = nullptr;
};

#endif


