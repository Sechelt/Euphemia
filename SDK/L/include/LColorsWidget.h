/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LColorsWidget
#define H_LColorsWidget

#include <SSettings.h>
#include <SPen.h>
#include <SBrush.h>

/*!
 * \brief   Shows a foreground color swatch partially overlaid on a background color swatch 
 *          and allows one, or the other, to be selected.
 *  
 * The foreground swatch reflects a pen color. 
 * The background swatch reflects a brush color. 
 *  
 * \sa LColorToolBar 
 *  
 * \author pharvey (2/21/23)
 */
class LColorControl : public QWidget
{
    Q_OBJECT
public:
    enum Swatchs
    {
        SwatchPen,
        SwatchBrush
    };

    LColorControl( QWidget *pParent );          /*!< will init based upon current pen/brush in g_SSetting        */

    Swatchs getSwatch() { return nSwatch; }

signals:
    void signalChanged( LColorControl::Swatchs );              
    void signalChanged( const QColor & );       /*!< active swatch changed - to SColorsWidget             */

public slots:
    void slotColorPicker();                     /*!< enter 'color picker' mode                                  */
    void slotChange( const QColor & );          /*!< update color of active swatch - from SColorsWidget   */
    void slotRefreshPen();                      /*!< update foreground swatch - from global context             */
    void slotRefreshBrush();                    /*!< update background swatch - from global context             */

protected:
    SPen *      pSettingPen;
    SBrush *    pSettingBrush;

    QSize       size            = QSize( 24, 24 );     
    bool        bColorPicker    = false;           /*!< true if we are in 'color picker' mode                      */
    Swatchs     nSwatch         = SwatchPen;
    QColor      colorPen;
    QColor      colorBrush;
    QRect       rectPen         = QRect( 0, 0, 16, 16 );
    QRect       rectBrush       = QRect( 0, 0, 16, 16 );

    void paintEvent( QPaintEvent *pEvent ) override;
    void resizeEvent( QResizeEvent *pEvent ) override;
    void mouseDoubleClickEvent( QMouseEvent *pEvent ) override;
    void mousePressEvent( QMouseEvent *pEvent ) override;

    QSize sizeHint() const override;

    void doCalcLayout();
};

/*!
 * \brief Color tool bar. 
 *  
 * This combines LColorControl with the tool bar widget for pen or brush. 
 * If foreground color is selected - the pen tool bar is shown. 
 * If background color is selected - the brush tool bar is shown. 
 *  
 * \sa LColorControl 
 *  
 * \author pharvey (3/15/23)
 */
class LColorToolBar : public QWidget
{
    Q_OBJECT
public:
    LColorToolBar( QWidget *pParent );

signals:
    void signalChanged( const QColor & );

public slots:
    void slotChange( const QColor & );

protected:
    QHBoxLayout *   pLayout;
    LColorControl * pColorControl;
    QWidget *       pPenToolBar         = nullptr;
    QWidget *       pBrushToolBar       = nullptr;

protected slots:
    void slotRefresh( LColorControl::Swatchs n );
};

/*!
 * \brief A widget for selecting a pen (foreground) or brush (background) color. 
 *  
 * Combines a SColorsWidget with a toolbar where the toolbar is used to 
 * indicate foreground or background. 
 * 
 * \author pharvey (3/20/23)
 */
class LColorsWidget : public QWidget
{
    Q_OBJECT
public:
    LColorsWidget( QWidget *pParent );
};

/*!
 * \brief
 * 
 * \author pharvey (3/30/23)
 */
class LColorsPenWidget : public QWidget
{
    Q_OBJECT
public:
    LColorsPenWidget( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SPen *              pPen;
    QPen                pen;
    WLineStyleComboBox *pStyle  = nullptr;
    QSpinBox *          pWidth  = nullptr;
    QToolButton *       pMore   = nullptr;

protected slots:
    void slotStyle( Qt::PenStyle n );
    void slotWidth( int n );
    void slotMore();
};

/*!
 * \brief 
 * 
 * \author pharvey (3/30/23)
 */
class LColorsBrushWidget : public QWidget
{
    Q_OBJECT
public:
    LColorsBrushWidget( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SBrush *                pBrush;
    QBrush                  brush;
    WBrushStyleComboBox *   pStyle  = nullptr;
    QToolButton *           pMore   = nullptr;

protected slots:
    void slotStyle( Qt::BrushStyle n );
    void slotMore();
};

#endif
