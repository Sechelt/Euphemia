/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */
#ifndef H_PASPolygonFilled
#define H_PASPolygonFilled

#include "SSetting.h"

/*!
 * \brief Preferences/Context settings for SPolygon.
 * 
 * \author pharvey (3/15/23)
 */
class PASPolygonFilled : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(Qt::FillRule nFillRule READ getFillRule WRITE setFillRule NOTIFY signalChanged)
public:
    PASPolygonFilled();

    void setFillRule( Qt::FillRule );
    void setSelected( Qt::FillRule );


    Qt::FillRule getFillRule()  { return nFillRule; }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent ) override;
    QWidget *getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    Qt::FillRule nFillRule = Qt::OddEvenFill;
};

/*!
 * \brief   Edit widget for use in a tool bar. 
 *          Edits SPolygon.
 *  
 * Horizontal layout - no labels.
 * 
 * \author pharvey (3/14/23)
 */
class PASPolygonFilledToolBar : public QWidget
{
    Q_OBJECT
public:
    PASPolygonFilledToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotFillRule( int );

protected:
    PASPolygonFilled *    pSetting;
    QComboBox *         pFillRule;
};

/*!
 * \brief   Edit widget for use in a panel.
 *          Edits SPolygon.
 *  
 * Form (Vertical) layout with labels. 
 *  
 * \author pharvey (3/14/23)
 */
class PASPolygonFilledPanel : public QWidget
{
    Q_OBJECT
public:
    PASPolygonFilledPanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotFillRule( int );

protected:
    PASPolygonFilled *    pSetting;
    QComboBox *         pFillRule;
};
#endif


