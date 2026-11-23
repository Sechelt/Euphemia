/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#ifndef H_DRSImage
#define H_DRSImage

#include <W.h>
#include <SSettings.h>

/*!
 * \brief Settings for DRImage.
 * 
 * \author pharvey (2026-11-21)
 */
class DRSImage : public SSetting 
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ getPixmap WRITE setPixmap NOTIFY signalChanged)
    Q_PROPERTY(W::WImagePlacements nPlacement READ getPlacement WRITE setPlacement NOTIFY signalChanged)
public:
    DRSImage();

    void setPixmap( const QPixmap & );
    void setPlacement( W::WImagePlacements );

    void setSelected( const QPixmap &, W::WImagePlacements );

    QPixmap             getPixmap()     { return pixmap;    }
    W::WImagePlacements getPlacement()  { return nPlacement;}

    QWidget *   getButton( QWidget *pParent ) override;
    QWidget *   getToolBar( QWidget *pParent ) override;
    QWidget *   getPanel( QWidget *pParent ) override;
    QWidget *   getSwatch( QWidget *pParent ) override;
    QToolBox *  getToolBox( QWidget *pParent ) override;

    void doSave( QDomDocument *pdomDoc, QDomElement *pdomElem ) override;
    void doLoad( QDomElement *pdomElem ) override;

protected:
    QPixmap pixmap;
    W::WImagePlacements nPlacement;
};

/*!
 * \brief A tool button to invoke a dialog/panel to edit.
 * 
 * Tool buttons are small so button always shows a standard image icon - not the selected pixmap (which will often be much larger). 
 * 
 * \author pharvey (9/29/25)
 */
class DRSImageButton : public QToolButton
{
    Q_OBJECT
public:
    DRSImageButton( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    DRSImage *pSetting;

protected slots:
    void slotClicked();
};

/*!
 * \brief Edit DRSImage using a widget which can be used in a tool bar. 
 *  
 * \author pharvey (9/29/25)
 */
class DRSImageToolBar : public QWidget
{
    Q_OBJECT
public:
    DRSImageToolBar( QWidget *pParent, bool bCompress = false );

public slots:
    void slotRefresh();

protected:
    DRSImage *          pSetting;
    DRSImageButton *    pImageButton;
    QComboBox *         pPlacement;

protected slots:
    void slotChangedPlacement( int nIndex );
};

/*!
 * \brief Panel for editing a DRSImage.
 *  
 * Vertical layout. 
 * Includes all details. 
 *  
 * \author pharvey (9/29/25)
 */
class DRSImagePanel : public QWidget
{
    Q_OBJECT
public:
    DRSImagePanel( QWidget *pParent = nullptr );
};

/*!
 * \brief Swatch (example) for a DRSImage.
 *  
 * \author pharvey (9/29/25)
 */
class DRSImageSwatch : public QWidget
{
    Q_OBJECT
public:
    DRSImageSwatch( QWidget *pParent );

public slots:
    void slotRefresh();

protected:
    DRSImage *  pSetting;
    QPixmap     pixmap;

    void paintEvent( QPaintEvent *pEvent );
    QSize sizeHint() const;
};

#endif

