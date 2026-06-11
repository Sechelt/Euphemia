/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_SPen
#define H_SPen

#include <SSettings.h>

#include "WColorButton.h"
#include "WLineStyleComboBox.h"
#include "WBrushStyleComboBox.h"
#include "WImageButton.h"

/*!
 * \brief Setting for a SPen.
 * 
 * \author pharvey (3/14/23)
 */
class SPen : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QPen pen READ getValue WRITE setValue NOTIFY signalChanged)
public:
    SPen();

    void setValue( const QPen & );
    void setSelected( const QPen & );

    QPen        getValue() { return pen; }
    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QPen pen;
};

/*!
 * \brief A button to invoke a dialog/panel to edit.
 * 
 * \author pharvey (12/27/24)
 */
class SPenButton : public QToolButton
{
    Q_OBJECT
public:
    SPenButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SPen *pPen;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit SPen using a widget which can be used in a tool bar. 
 *  
 * Horizontal layout with subset of options. 
 * Includes 'More' button to invoke dialog to edit more details. 
 * 
 * \author pharvey (3/14/23)
 */
class SPenToolBar : public QWidget
{
    Q_OBJECT
public:
    SPenToolBar( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SPen *              pPen;
    QPen                pen;
    WColorButton *      pColor  = nullptr;
    WLineStyleComboBox *pStyle  = nullptr;
    QSpinBox *          pWidth  = nullptr;
    QToolButton *       pMore   = nullptr;

protected slots:
    void slotColor( const QColor &color );
    void slotStyle( Qt::PenStyle n );
    void slotWidth( int n );
    void slotMore();
};

/*!
 * \brief Panel for editing a SPen.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (3/14/23)
 */
class SPenPanel : public QWidget
{
    Q_OBJECT
public:
    SPenPanel( QWidget *pParent = nullptr );

public slots:
    void slotRefresh();

protected:
    SPen *pPen;
    QPen  pen;
    // pen
    QComboBox *         pCapStyle;
    WColorButton *      pColor;
    QComboBox *         pJoinStyle;
    QSpinBox *          pMiterLimit;
    WLineStyleComboBox *pStyle;
    QSpinBox *          pWidth;
    // brush (if any) to fill strokes
    WColorButton *          pColorBrush;
    WBrushStyleComboBox *   pStyleBrush;
    WImageButton *          pImageBrush;

protected slots:
    // pen
    void slotCapStyle( int nIndex );
    void slotColor( const QColor &color );
    void slotJoinStyle( int nIndex );
    void slotMiterLimit( int n );
    void slotStyle( Qt::PenStyle n );
    void slotWidth( int n );
    // brush (if any) to fill strokes
    void slotColorBrush( const QColor &color );
    void slotStyleBrush( Qt::BrushStyle n );
    void slotImageBrush();
};

/*!
 * \brief Swatch (example) for a SPen.
 * 
 * \author pharvey (3/14/23)
 */
class SPenSwatch : public QWidget
{
    Q_OBJECT
public:
    SPenSwatch( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SPen *pPen;
    QPen  pen;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

#endif


