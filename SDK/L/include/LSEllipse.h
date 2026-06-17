/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LSEllipse
#define H_LSEllipse

#include "SSetting.h"

/*!
 * \brief Preferences/Context settings for LSEllipse.
 * 
 * \author pharvey (3/15/23)
 */
class LSEllipse : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(bool bBind READ getBind WRITE setBind NOTIFY signalChanged)
public:
    LSEllipse();

    void setBind( bool );
    void setSelected( bool );

    bool            getBind()       { return bBind;     }

    QWidget *getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *getToolBar( QWidget *pParent ) override;
    QWidget *getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    bool            bBind           = false;            /*!< true = make squares/circles                                */
};

/*!
 * \brief   Edit widget for use in a tool bar. 
 *          Edits LSEllipse.
 *  
 * Horizontal layout.
 * 
 * \author pharvey (3/14/23)
 */
class LSEllipseToolBar : public QWidget
{
    Q_OBJECT
public:
    LSEllipseToolBar( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
    void slotBind( Qt::CheckState );

protected:
    LSEllipse *    pSetting;

    QCheckBox * pBind;
};

/*!
 * \brief   Edit widget for use in a dialog or a dock.
 *          Edits LSEllipse.
 *  
 * Form (Vertical) layout with labels.
 * 
 * \author pharvey (3/14/23)
 */
class LSEllipsePanel : public QWidget
{
    Q_OBJECT
public:
    LSEllipsePanel( QWidget *p );

public slots:
    void slotRefresh();

protected slots:
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    void slotBind( Qt::CheckState );
#else
    void slotBind( int );
#endif

protected:
    LSEllipse *    pSetting;

    QCheckBox * pBind;
};

#endif


