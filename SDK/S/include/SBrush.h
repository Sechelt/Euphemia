/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_SBrush
#define H_SBrush

#include <SSettings.h>

#include <WColorButton.h>
#include <WBrushStyleComboBox.h>
#include <WImageButton.h>

/*!
 * \brief Setting for a SBrush.
 * 
 * \author pharvey (3/14/23)
 */
class SBrush : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QBrush brush READ getValue WRITE setValue NOTIFY signalChanged)
public:
    SBrush();

    void    setValue( const QBrush & );
    void    setSelected( const QBrush & );  // called when an object has been selected

    QBrush      getValue() { return brush; }
    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QBrush  brush;              // current brush
};

/*!
 * \brief A button to invoke a dialog/panel to edit.
 * 
 * \author pharvey (12/27/24)
 */
class SBrushButton : public QToolButton
{
    Q_OBJECT
public:
    SBrushButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SBrush *pBrush;

    QPixmap getIcon();

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit SBrush using a widget which can be used in a tool bar. 
 *  
 * Horizontal layout with subset of options. 
 * Includes 'More' button to invoke dialog to edit more details. 
 * 
 * \author pharvey (3/14/23)
 */
class SBrushToolBar : public QWidget
{
    Q_OBJECT
public:
    SBrushToolBar( QWidget *pParent, bool bCompress = false );

public slots:
    void slotRefresh();

protected:
    SBrush *               pSettingsBrush;
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
 * \brief Panel for editing a SBrush.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (3/14/23)
 */
class SBrushPanel : public QWidget
{
    Q_OBJECT
public:
    SBrushPanel( QWidget *pParent = nullptr );

public slots:
    void slotRefresh();

protected:
    SBrush *               pSettingsBrush;
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
 * \brief Swatch (example) for a SBrush.
 *  
 * \author pharvey (3/14/23)
 */
class SBrushSwatch : public QWidget
{
    Q_OBJECT
public:
    SBrushSwatch( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    SBrush *   pSettingsBrush;
    QBrush      brush;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

#endif

