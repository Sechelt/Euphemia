/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_SArrow
#define H_SArrow

#include <SSettings.h>

/*!
 * \brief Setting for a SArrow.
 * 
 * \author pharvey (3/14/23)
 */
class SArrow : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(qreal nSize READ getValue WRITE setValue NOTIFY signalChanged)
public:
    SArrow();

    void setValue( qreal );
    void setSelected( qreal );

    qreal       getValue() { return nSize; }
    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    qreal nSize;
};

/*!
 * \brief A button to invoke a dialog/panel to edit.
 * 
 * \author pharvey (12/27/24)
 */
class SArrowButton : public QToolButton
{
    Q_OBJECT
public:
    SArrowButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SArrow *pArrow;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit SArrow using a widget which can be used in a tool bar. 
 *  
 * Horizontal layout with subset of options. 
 * Includes 'More' button to invoke dialog to edit more details. 
 * 
 * \author pharvey (3/14/23)
 */
class SArrowToolBar : public QWidget
{
    Q_OBJECT
public:
    SArrowToolBar( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SArrow *            pArrow;
    qreal               nSize;
    QDoubleSpinBox *    pSize   = nullptr;
    QToolButton *       pMore   = nullptr;

protected slots:
    void slotSize( qreal n );
    void slotMore();
};

/*!
 * \brief Panel for editing a SArrow.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (3/14/23)
 */
class SArrowPanel : public QWidget
{
    Q_OBJECT
public:
    SArrowPanel( QWidget *pParent = nullptr );

public slots:
    void slotRefresh();

protected:
    SArrow *pArrow;
    qreal   nSize;
    QDoubleSpinBox *          pSize;

protected slots:
    void slotSize( qreal n );
};

/*!
 * \brief Swatch (example) for a SArrow.
 * 
 * \author pharvey (3/14/23)
 */
class SArrowSwatch : public QWidget
{
    Q_OBJECT
public:
    SArrowSwatch( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SArrow *pArrow;
    qreal   nSize;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

#endif


