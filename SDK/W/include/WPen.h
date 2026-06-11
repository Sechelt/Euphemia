/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_WPen
#define H_WPen

#include "WColorButton.h"
#include "WLineStyleComboBox.h"
#include "WBrushStyleComboBox.h"
#include "WImageButton.h"

/*!
 * \brief Tool bar button giving some indication of the QPen.
 * 
 * Click will invoke \sa WPenDialog.
 * Use when space is very limited.
 * 
 * \author pharvey (1/1/25)
 */
class WPenButton : public QToolButton
{
    Q_OBJECT
public:
    WPenButton( const QPen &pen, QWidget *pParent );

    QPen getPen() { return pen; }

signals:
    void signalChanged( const QPen &pen );

public slots:
    void slotRefresh( const QPen &pen );

protected:
    QPen pen;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit QPen in a tool bar.
 * 
 * Includes a 'more' button to invoke \sa WPenDialog.
 * 
 * \author pharvey (1/1/25)
 */
class WPenToolBar : public QWidget
{
    Q_OBJECT
public:
    WPenToolBar( const QPen &pen, QWidget *pParent );

    void setPen( const QPen &pen ) { slotRefresh( pen ); }

    QPen getPen() { return pen; }

signals:
    void signalChanged( const QPen &pen );

public slots:
    void slotRefresh( const QPen &pen );

protected:
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
 * \brief Full edit of a QPen.
 * 
 * \author pharvey (1/1/25)
 */
class WPenPanel : public QWidget
{
    Q_OBJECT
public:
    WPenPanel( const QPen &pen, QWidget *pParent = nullptr );

    QPen getPen() { return pen; }

signals:
    void signalChanged( const QPen &pen );

public slots:
    void slotRefresh( const QPen &pen );

protected:
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
 * \brief An example of the QPen.
 * 
 * \author pharvey (1/1/25)
 */
class WPenSwatch : public QWidget
{
    Q_OBJECT
public:
    WPenSwatch( const QPen &pen, QWidget *pParent );

public slots:
    void slotRefresh( const QPen &pen );

protected:
    QPen  pen;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

/*!
 * \brief Dialog containing \sa WPenPanel.
 * 
 * \author pharvey (1/1/25)
 */
class WPenDialog : public QDialog
{
    Q_OBJECT
public:
    WPenDialog( const QPen &pen, QWidget *pParent = nullptr );

    QPen getPen() { return pPanel->getPen(); }
    static QPen doEdit( const QPen &pen, QWidget *pParent );

protected:
    WPenPanel *pPanel;
};

#endif


