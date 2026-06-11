/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_LSBackground
#define H_LSBackground

#include <WColorButton.h>
#include <WBrushStyleComboBox.h>
#include <WImageButton.h>

#include "SSettings.h"

/*!
 * \brief Preferences/Context settings for background.
 *  
 * The background can be transparent or filled with a brush (brushBackground). 
 * Transparency can be represented by the standard block pattern, by some other brush or by nothing at all (brushTransparency). 
 * brushTransparency is used when brushBackground alpha is 0 (indicating transparent). 
 *  
 * Preferences 
 *  
 * The following are edited/stored/retrieved for app init at startup; 
 *  brushBackground 
 *  brushTransparency 
 *  
 * Context 
 *  
 * The following are provided as a global; 
 *  brushTransparency
 *  
 * The following are provided by the file; 
 *  brushBackground 
 *  
 * \author pharvey (6/16/23)
 */
class LSBackground : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QBrush brushTransparency READ getTransparency WRITE setTransparency NOTIFY signalChanged)
public:
    LSBackground();

    void setTransparency( const QBrush & );
    void setSelected( const QBrush & );

    QBrush      getTransparency()   { return brushTransparency; }

    QWidget *   getButton( QWidget * /* pParent */ ) { return nullptr; }
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QBrush  brushTransparency;                      /*!< brush to use to represent transparency - default is block pattern but a solid color may be easier to work with     */
};

/*!
 * \brief   Widget for editing \sa LSBackground.
 * 
 * \author pharvey (6/16/23)
 */
class LSBackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    LSBackgroundWidget( QWidget *pParent, Qt::Orientation n );

public slots:
    void slotRefresh();

protected:
    LSBackground *          pSetting;
    // brushBackground
    WColorButton *          pColor;         /*!< a transparent color (alpha==0) and no texture will implicitly indicate block pattern  */
    WBrushStyleComboBox *   pStyle;
    WImageButton *          pImage;

protected slots:
    void slotColor( const QColor &color );
    void slotStyle( Qt::BrushStyle n );
    void slotImage();
};

#endif


