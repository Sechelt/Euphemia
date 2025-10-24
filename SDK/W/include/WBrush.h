/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */
#ifndef H_WBrush
#define H_WBrush

#include <WColorButton.h>
#include <WBrushStyleComboBox.h>
#include <WImageButton.h>

/*!
 * \brief Tool bar button giving some indication of the QBrush.
 * 
 * Click will invoke \sa WBrushDialog.
 * Use when space is very limited.
 * 
 * \author pharvey (1/1/25)
 */
class WBrushButton : public QToolButton
{
    Q_OBJECT
public:
    WBrushButton( const QBrush &brush, QWidget *pParent );

    QBrush getBrush() { return brush; }

signals:
    void signalChanged( const QBrush &brush );

public slots:
    void slotRefresh( const QBrush &brush );

protected:
    QBrush brush;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit QBrush in a tool bar.
 * 
 * Includes a 'more' button to invoke \sa WBrushDialog.
 * 
 * \author pharvey (1/1/25)
 */
class WBrushToolBar : public QWidget
{
    Q_OBJECT
public:
    WBrushToolBar( const QBrush &brush, QWidget *pParent );

    QBrush getBrush() { return brush; }

signals:
    void signalChanged( const QBrush &brush );

public slots:
    void slotRefresh( const QBrush &brush );

protected:
    QBrush                  brush;
    WColorButton *          pColor  = nullptr;
    WBrushStyleComboBox *   pStyle  = nullptr;
    WImageButton *          pImage  = nullptr;
    QToolButton *           pMore   = nullptr;

protected slots:
    void slotColor( const QColor &color );
    void slotStyle( Qt::BrushStyle n );
    void slotImage();
    void slotMore();
};

/*!
 * \brief Full edit of a QBrush.
 * 
 * \author pharvey (1/1/25)
 */
class WBrushPanel : public QWidget
{
    Q_OBJECT
public:
    WBrushPanel( const QBrush &brush, QWidget *pParent = nullptr );

    QBrush getBrush() { return brush; }

signals:
    void signalChanged( const QBrush &brush );

public slots:
    void slotRefresh( const QBrush &brush );

protected:
    QBrush                  brush;
    WColorButton *          pColor;
    WBrushStyleComboBox *   pStyleComboBox;
    WImageButton *          pImage;

protected slots:
    void slotColor( QColor color );
    void slotStyle( Qt::BrushStyle n );
    void slotImage();
};

/*!
 * \brief An example of the QBrush.
 * 
 * \author pharvey (1/1/25)
 */
class WBrushSwatch : public QWidget
{
    Q_OBJECT
public:
    WBrushSwatch( const QBrush &brush, QWidget *pParent );

public slots:
    void slotRefresh( const QBrush &brush );

protected:
    QBrush      brush;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

/*!
 * \brief Dialog containing \sa WBrushPanel.
 * 
 * \author pharvey (1/1/25)
 */
class WBrushDialog : public QDialog
{
    Q_OBJECT
public:
    WBrushDialog( const QBrush &brush, QWidget *pParent = nullptr );

    QBrush getBrush() { return pPanel->getBrush(); }
    static QBrush doEdit( const QBrush &brush, QWidget *pParent );

protected:
    WBrushPanel *pPanel;
};

#endif

