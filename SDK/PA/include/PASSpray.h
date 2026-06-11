/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_PASSpray
#define H_PASSpray

#include "SSetting.h"

/*!
 * \brief Preferences/Context settings for PASSpray.
 * 
 * \author pharvey (3/15/23)
 */
class PASSpray : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(int nRadius READ getRadius WRITE setRadius NOTIFY signalChanged)
    Q_PROPERTY(int nPoints READ getPoints WRITE setPoints NOTIFY signalChanged)
public:
    PASSpray();

    void setRadius( int );
    void setPoints( int );
    void setSelected( int, int );

    int getRadius()         { return nRadius;   }
    int getPoints()         { return nPoints;   }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent ) override;
    QWidget *getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    int nRadius = 10;
    int nPoints = 5;
};

/*!
 * \brief   Edit widget for use in a tool bar. 
 *          Edits PASSpray.
 *  
 * Horizontal layout.
 * 
 * \author pharvey (3/14/23)
 */
class PASSprayToolBar : public QWidget
{
    Q_OBJECT
public:
    PASSprayToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotRadius( int );
    void slotPoints( int );

protected:
    PASSpray *    pSetting;

    QSpinBox *  pRadius;
    QSpinBox *  pPoints;
};

/*!
 * \brief   Edit widget for use in a dialog or a dock.
 *          Edits PASSpray.
 *  
 * Vertical layout.
 * 
 * \author pharvey (3/14/23)
 */
class PASSprayPanel : public QWidget
{
    Q_OBJECT
public:
    PASSprayPanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotRadius( int );
    void slotPoints( int );

protected:
    PASSpray *    pSetting;

    QSpinBox *  pRadius;
    QSpinBox *  pPoints;
};

#endif


